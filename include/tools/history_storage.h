#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

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


    bool saveAudioData(const QByteArray &audioData, const QString &conversationId, const QString &conversationContent)
    {
        // 创建音频文件路径
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
        QString filePath =
            QString(customIniFilePath("/audios") + "/audio_%1_%2.wav").arg(conversationId).arg(timestamp);

        // 保存音频数据到文件
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "无法打开文件进行写入:" << filePath;
            return false;
        }
        file.write(audioData);
        file.close();

        // 保存元数据到数据库
        if (!db.isOpen() && !db.open())
        {
            qDebug() << "无法打开数据库:" << db.lastError().text();
            return false;
        }

        QSqlQuery query;
        query.prepare("CREATE TABLE IF NOT EXISTS audio_history (id INTEGER PRIMARY KEY AUTOINCREMENT, conversation_id "
                      "TEXT, file_path TEXT, timestamp TEXT, conversationContent TEXT)");
        if (!query.exec())
        {
            qDebug() << "创建表失败:" << query.lastError().text();
            return false;
        }

        query.prepare("INSERT INTO audio_history (conversation_id, file_path, timestamp,conversationContent) VALUES "
                      "(:conversation_id, "
                      ":file_path, :timestamp,:conversationContent)");
        query.bindValue(":conversation_id", conversationId);
        query.bindValue(":file_path", filePath);
        query.bindValue(":timestamp", timestamp);
        query.bindValue(":conversationContent", conversationContent);
        if (!query.exec())
        {
            qDebug() << "插入数据失败:" << query.lastError().text();
            return false;
        }

        return true;
    }

    bool readAudioData(const QString &conversationId, QByteArray &audioData)
    {
        if (!db.isOpen() && !db.open())
        {
            qDebug() << "无法打开数据库:" << db.lastError().text();
            return false;
        }

        QSqlQuery query;
        query.prepare(
            "SELECT file_path FROM audio_history WHERE conversation_id = :conversation_id ORDER BY timestamp");
        query.bindValue(":conversation_id", conversationId);
        if (!query.exec())
        {
            qDebug() << "查询数据失败:" << query.lastError().text();
            return false;
        }

        QString filePath{};
        while (query.next())
        {
            filePath = query.value(0).toString();
        }

        if (filePath.isEmpty())
        {
            qDebug() << "未找到对应的音频文件路径";
            return false;
        }
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "无法打开文件进行读取:" << filePath;
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
            qDebug() << "无法打开数据库:" << db.lastError().text();
        }
    }

    QSqlDatabase db;


    QString customIniFilePath(QString subPath) const
    {
        QString dirPath = QCoreApplication::applicationDirPath() + "/Data" + subPath;
        QDir dir;
        qDebug() << "创建路径 " << dirPath;
        if (!dir.exists(dirPath))
        {
            dir.mkpath(dirPath);
        }
        return dirPath;
    }
};


#endif
