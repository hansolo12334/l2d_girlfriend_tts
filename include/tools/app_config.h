// AppConfig.h
#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include <QSettings>
#include<QCoreApplication>
#include <QDir>

class AppConfig : public QObject {
    Q_OBJECT

public:
    static AppConfig& instance() {
        static AppConfig instance;
        return instance;
    }

    bool isEnableMica() const { return _isEnableMica; }
    void setIsEnableMica(bool enable) { _isEnableMica = enable; emit configChanged(); }

    bool isEnableTTS() const { return _isEnableTTS; }
    void setIsEnableTTS(bool enable) { _isEnableTTS = enable; emit configChanged(); }

    bool isEnableLogging() const { return _isEnableLogging; }
    void setIsEnableLogging(bool enable) { _isEnableLogging = enable; emit configChanged(); }

    QString getPromotSentence() const { return _promot_sentence; }
    void setPromotSentence(QString promot_sentence){ _promot_sentence = promot_sentence; emit configChanged(); }

    int getTokenSize() const { return _token_size; }
    void setTokenSize(int token_size){ _token_size = token_size; emit configChanged(); }

    int getTTsVoiceChoise() const { return _vice_choise; }
    void setTTsVoiceChoise(int choise){_vice_choise=choise; emit configChanged();}

    int getConversationTimes() const { return _conversationTimes; }
    void setConversationTimes(int times){_conversationTimes=times; emit configChanged();}


    void loadSettings() {
        qDebug() << "正在加载设置数据..";
        QSettings settings(customIniFilePath(),  QSettings::IniFormat);
        _isEnableMica = settings.value("enableMica", false).toBool();
        _isEnableTTS = settings.value("enableTTS", true).toBool();
        _isEnableLogging = settings.value("enableLogging", false).toBool();
        _promot_sentence = settings.value("promot_sentence", "亲爱的，我是你的性感女友，我会为了你做任何事情。").toString();
        _token_size = settings.value("token_size", 50).toInt();
        _vice_choise = settings.value("voice_choise", 1).toInt();
        _conversationTimes = settings.value("conversationTimes", 0).toInt();
        // qDebug() << "_isEnableTTS " << _isEnableTTS;
        // qDebug() << "_promot_sentence " << _promot_sentence;
    }

    void saveSettings() const {
        QSettings settings(customIniFilePath(),  QSettings::IniFormat);
        qDebug() << "正在保存设置数据..";
        settings.setValue("enableMica", _isEnableMica);
        settings.setValue("enableTTS", _isEnableTTS);
        settings.setValue("enableLogging", _isEnableLogging);
        settings.setValue("promot_sentence", _promot_sentence);
        settings.setValue("token_size", _token_size);
        settings.setValue("voice_choise", _vice_choise);
        settings.setValue("conversationTimes", _conversationTimes);
    }

signals:
    void configChanged();

private:
    AppConfig() { loadSettings(); }

    QString customIniFilePath() const {
        QString dirPath =  QCoreApplication::applicationDirPath() + "/Config";
        QDir dir;
        qDebug() << "创建路径 "<<dirPath;
        if (!dir.exists(dirPath))
        {
            dir.mkpath(dirPath);
        }
        return dirPath + "/test.ini";
    }


    bool _isEnableMica = false;
    bool _isEnableTTS = true;
    bool _isEnableLogging = false;
    int _token_size = 50;
    int _vice_choise = 1;
    int _conversationTimes = 0;
    QString _promot_sentence{};
};

#endif // APPCONFIG_H
