// AppConfig.h
#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include <QSettings>
#include<QCoreApplication>
#include <QDir>
#include "app_log.h"

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

    QString getOllamaApiAddress() const { return _ollama_api_address; }
    void setOllamaApiAddress(QString ollama_api_address){ _ollama_api_address = ollama_api_address; emit configChanged(); }



    int getTokenSize() const { return _token_size; }
    void setTokenSize(int token_size){ _token_size = token_size; emit configChanged(); }

    int getTTsVoiceChoise() const { return _vice_choise; }
    void setTTsVoiceChoise(int choise){_vice_choise=choise; emit configChanged();}

    int getConversationTimes() const { return _conversationTimes; }
    void setConversationTimes(int times){_conversationTimes=times; emit configChanged();}


    void loadSettings();

    void saveSettings();

    void getModelList();

signals:
    void configChanged();

private:
    AppConfig() { loadSettings(); }

    QString customIniFilePath();

    //确保只初始化一次
    bool _is_init = false;

    bool _isEnableMica = false;
    bool _isEnableTTS = true;
    bool _isEnableLogging = false;
    int _token_size = 50;
    int _vice_choise = 1;
    int _conversationTimes = 0;
    QString _promot_sentence{};
    QString _ollama_api_address{};
};

#endif // APPCONFIG_H
