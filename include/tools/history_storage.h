#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

#include "app_log.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class history_storage : public QObject
{
    Q_OBJECT

public:
    static history_storage &instance()
    {
        static history_storage instance;
        return instance;
    }

    // bool saveConversationData(const QString &conversationId, const QString &conversationContent,QString &question)
    // {

    // }

    bool saveConversationData(const QByteArray &audioData,
                              const QString &conversationId,
                              const QString &conversationContent,
                              const QString &question,
                              bool ISsaveAudioData)
    {

        // 创建音频文件路径
        QString filePath = "";
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
        if (ISsaveAudioData)
        {
            filePath = QString(customIniFilePath("/audios") + "/audio_%1_%2.wav").arg(conversationId).arg(timestamp);

            // 保存音频数据到文件
            QFile file(filePath);
            if (!file.open(QIODevice::WriteOnly))
            {
                APP_LOG_ERROR("无法打开文件进行写入: " << filePath);
                return false;
            }
            file.write(audioData);
            file.close();
        }


        // 保存元数据到数据库
        if (!db.isOpen() && !db.open())
        {
            APP_LOG_ERROR("无法打开数据库: " << db.lastError().text());
            return false;
        }

        QSqlQuery query;
        query.prepare("CREATE TABLE IF NOT EXISTS audio_history (id INTEGER PRIMARY KEY AUTOINCREMENT, conversation_id "
                      "TEXT, file_path TEXT, timestamp TEXT, question TEXT,conversationContent TEXT)");
        if (!query.exec())
        {
            APP_LOG_ERROR("创建表失败: " << query.lastError().text());
            return false;
        }

        //查询是否存在相同 conversation_id
        query.prepare("SELECT COUNT(*) FROM audio_history WHERE conversation_id = :conversation_id");
        query.bindValue(":conversation_id", conversationId);
        if (!query.exec())
        {
            APP_LOG_ERROR("查询数据失败: " << query.lastError().text());
            return false;
        }

        query.next();
        int count = query.value(0).toInt();

        if (count > 0)
        {
            // 否存在相同 conversation_id 更新已有记录
            query.prepare(
                "UPDATE audio_history SET file_path = :file_path, question = :question, "
                "conversationContent = :conversationContent WHERE conversation_id = :conversation_id");
            query.bindValue(":file_path", filePath);
            query.bindValue(":question", question);
            query.bindValue(":conversationContent", conversationContent);
        }
        else
        {
            // 不否存在相同 conversation_id 插入新记录
            query.prepare(
                "INSERT INTO audio_history (conversation_id, file_path, timestamp, question, conversationContent) "
                "VALUES (:conversation_id, :file_path, :timestamp, :question, :conversationContent)");

            query.bindValue(":conversation_id", conversationId);
            query.bindValue(":file_path", filePath);
            query.bindValue(":timestamp", timestamp);
            query.bindValue(":question", question);
            query.bindValue(":conversationContent", conversationContent);
        }

        // query.prepare("INSERT INTO audio_history (conversation_id, file_path, timestamp,question,conversationContent) VALUES "
        //               "(:conversation_id, "
        //               ":file_path, :timestamp,:question,:conversationContent)");

        if (!query.exec())
        {
            APP_LOG_ERROR("插入数据失败: " << query.lastError().text());
            return false;
        }

        return true;
    }

    bool readConversationString(const QString &conversationId, QString &data)
    {
        if (!db.isOpen() && !db.open())
        {
            APP_LOG_ERROR("无法打开数据库: " << db.lastError().text());
            return false;
        }

        QSqlQuery query;
        query.prepare("SELECT conversationContent FROM audio_history WHERE conversation_id = :conversation_id ORDER BY "
                      "timestamp");
        query.bindValue(":conversation_id", conversationId);
        if (!query.exec())
        {
            APP_LOG_ERROR("查询数据失败: " << query.lastError().text());
            return false;
        }

        QString conversationContent{};
        while (query.next())
        {
            conversationContent = query.value(0).toString();
        }

        if (conversationContent.isEmpty())
        {
            APP_LOG_ERROR("未找到对应的对话");
            return false;
        }
        data = conversationContent;
        return true;
    }

    bool readAudioData(const QString &conversationId, QByteArray &audioData)
    {
        if (!db.isOpen() && !db.open())
        {
            APP_LOG_ERROR("无法打开数据库: " << db.lastError().text());
            return false;
        }

        QSqlQuery query;
        query.prepare(
            "SELECT file_path FROM audio_history WHERE conversation_id = :conversation_id ORDER BY timestamp");
        query.bindValue(":conversation_id", conversationId);
        if (!query.exec())
        {
            APP_LOG_ERROR("查询数据失败: " << query.lastError().text());
            return false;
        }

        QString filePath{};
        while (query.next())
        {
            filePath = query.value(0).toString();
        }

        if (filePath.isEmpty())
        {
            APP_LOG_ERROR("未找到对应的音频文件路径");
            return false;
        }
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            APP_LOG_ERROR("无法打开文件进行读取: " << filePath);
            return false;
        }

        audioData = file.readAll();
        file.close();

        return true;
    }


private:
    history_storage()
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(customIniFilePath("") + "/chat_history.db");
        if (!db.open())
        {
            APP_LOG_ERROR("无法打开数据库: " << db.lastError().text());
        }
    }

    QSqlDatabase db;


    QString customIniFilePath(QString subPath) const
    {
        QString dirPath = QCoreApplication::applicationDirPath() + "/Data" + subPath;
        QDir dir;
        if (!dir.exists(dirPath))
        {
            APP_LOG_DEBUG("创建路径 " << dirPath);
            dir.mkpath(dirPath);
        }
        return dirPath;
    }
};


#endif
