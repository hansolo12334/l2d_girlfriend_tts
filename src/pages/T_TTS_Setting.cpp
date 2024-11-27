#include "T_TTS_Setting.h"

#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QProcess>
#include <QVBoxLayout>

#include "ElaApplication.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaLog.h"
#include "ElaRadioButton.h"
#include "ElaScrollPageArea.h"
#include "ElaSlider.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToggleSwitch.h"
#include "ElaWindow.h"

#include "app_config.h"


T_TTS_Setting::T_TTS_Setting(QWidget *parent) : T_BasePage(parent)
{

    //tts进程
    tts_progress = new QProcess(this);
    // 预览窗口标题
    ElaWindow *window = dynamic_cast<ElaWindow *>(parent);
    setWindowTitle("TTS Setting");


    //tts

    //bat启动位置
    ElaLineEdit *tts_start_bat_edit = new ElaLineEdit(this);
    tts_start_bat_edit->setText("D:\\Qt_project\\2024\\fish-speech-1.4.2\\open_server.bat");
    tts_start_bat_edit->setMaximumWidth(500);

    ElaToggleSwitch *enable_tts_bat_switch = new ElaToggleSwitch(this);
    enable_tts_bat_switch->setIsToggled(false);
    connect(enable_tts_bat_switch, &ElaToggleSwitch::toggled, this, [=](bool checked) {
        open_tts_bat(checked, tts_start_bat_edit->text());
    });
    // AppConfig::instance().setPromotSentence(promot_sentence_edit->text());

    ElaScrollPageArea *tts_start_bat_Area = new ElaScrollPageArea(this);
    QHBoxLayout *tts_start_bat_Layout = new QHBoxLayout(tts_start_bat_Area);
    ElaText *tts_start_bat_Text = new ElaText("tts脚本启动位置", this);
    tts_start_bat_Text->setWordWrap(true);
    tts_start_bat_Text->setTextPixelSize(15);
    tts_start_bat_Layout->addWidget(tts_start_bat_Text);
    // tts_start_bat_Layout->addStretch();
    tts_start_bat_Layout->addWidget(tts_start_bat_edit);
    tts_start_bat_Layout->addWidget(enable_tts_bat_switch);

    //tts 启动
    _enableTTsButton = new ElaToggleSwitch(this);
    _enableTTsButton->setIsToggled(true);
    // qDebug() << "启用 tts";
    APP_LOG_DEBUG("启用 tts");

    ElaScrollPageArea *ttsSwitchArea = new ElaScrollPageArea(this);
    QHBoxLayout *ttsSwitchLayout = new QHBoxLayout(ttsSwitchArea);
    ElaText *enableTTsText = new ElaText("启用TTS", this);
    enableTTsText->setWordWrap(false);
    enableTTsText->setTextPixelSize(15);
    ttsSwitchLayout->addWidget(enableTTsText);
    ttsSwitchLayout->addStretch();
    ttsSwitchLayout->addWidget(_enableTTsButton);
    connect(_enableTTsButton, &ElaToggleSwitch::toggled, this, [&](bool checked) {
        //共用的全局设置数据
        AppConfig::instance().setIsEnableTTS(checked);
        AppConfig::instance().saveSettings();
    });

    //tts音色
    ElaComboBox *ttsCharacterSelect_combox = new ElaComboBox(this);
    ttsCharacterSelect_combox->addItem("1");
    ttsCharacterSelect_combox->addItem("2");
    ttsCharacterSelect_combox->addItem("3");

    ttsCharacterSelect_combox->setCurrentIndex(AppConfig::instance().getTTsVoiceChoise() - 1);

    ElaScrollPageArea *ttsCharacterSelect_Area = new ElaScrollPageArea(this);
    QHBoxLayout *ttsCharacterSelect_Layout = new QHBoxLayout(ttsCharacterSelect_Area);
    ElaText *ttsCharacterSelect_Text = new ElaText("TTS音色选择", this);
    ttsCharacterSelect_Text->setWordWrap(false);
    ttsCharacterSelect_Text->setTextPixelSize(15);
    ttsCharacterSelect_Layout->addWidget(ttsCharacterSelect_Text);
    ttsCharacterSelect_Layout->addStretch();
    ttsCharacterSelect_Layout->addWidget(ttsCharacterSelect_combox);
    connect(ttsCharacterSelect_combox, &ElaComboBox::currentIndexChanged, this, [=]() {
        AppConfig::instance().setTTsVoiceChoise(ttsCharacterSelect_combox->currentText().toInt());
        AppConfig::instance().saveSettings();
    });



    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("TTS文字转语音设置");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(tts_start_bat_Area);
    centerLayout->addWidget(ttsSwitchArea);
    centerLayout->addWidget(ttsCharacterSelect_Area);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

}


T_TTS_Setting::~T_TTS_Setting()
{

}

void T_TTS_Setting::open_tts_bat(bool enable, QString bat_path)
{
    if (enable)
    {
        APP_LOG_DEBUG("启动bat脚本: " << enable << " bat_path " << bat_path);
        tts_progress->start("cmd.exe", {"/c", "start", bat_path});
    }
    else
    {
        APP_LOG_DEBUG("关闭bat脚本: " << enable << " bat_path " << bat_path);
        // tts_progress->terminate();
    }
}
