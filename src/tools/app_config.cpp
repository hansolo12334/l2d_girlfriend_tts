#include "app_config.h"


void AppConfig::loadSettings()
{
    if(_is_init){
        // qDebug() << "已经初始化";
        APP_LOG_DEBUG("已经初始化");
        return;
    }
    // qDebug() << "正在加载设置数据..";
    APP_LOG_DEBUG("正在加载设置数据..");

    QSettings settings(customIniFilePath(), QSettings::IniFormat);
    _isEnableMica = settings.value("enableMica", false).toBool();
    _isEnableTTS = settings.value("enableTTS", true).toBool();
    _isEnableLogging = settings.value("enableLogging", false).toBool();
    _promot_sentence = settings.value("promot_sentence", "亲爱的，我是你的性感女友，我会为了你做任何事情。").toString();
    _token_size = settings.value("token_size", 50).toInt();
    _vice_choise = settings.value("voice_choise", 1).toInt();
    _conversationTimes = settings.value("conversationTimes", 0).toInt();
    // qDebug() << "_isEnableTTS " << _isEnableTTS;
    // qDebug() << "_promot_sentence " << _promot_sentence;
    _is_init = true;
}

void AppConfig::saveSettings()
{
    QSettings settings(customIniFilePath(), QSettings::IniFormat);
    // qDebug() << "正在保存设置数据..";
    APP_LOG_DEBUG("正在保存设置数据..");
    settings.setValue("enableMica", _isEnableMica);
    settings.setValue("enableTTS", _isEnableTTS);
    settings.setValue("enableLogging", _isEnableLogging);
    settings.setValue("promot_sentence", _promot_sentence);
    settings.setValue("token_size", _token_size);
    settings.setValue("voice_choise", _vice_choise);
    settings.setValue("conversationTimes", _conversationTimes);
}



QString AppConfig::customIniFilePath()
{
    QString dirPath =  QCoreApplication::applicationDirPath() + "/Config";
    QDir dir;
    if (!dir.exists(dirPath))
    {
        APP_LOG_DEBUG("创建路径 " << dirPath);
        // qDebug() << "创建路径 " << dirPath;
        dir.mkpath(dirPath);
    }
    return dirPath + "/test.ini";
}
