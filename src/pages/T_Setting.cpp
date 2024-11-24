#include "T_Setting.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaApplication.h"
#include "ElaComboBox.h"
#include "ElaLog.h"
#include "ElaRadioButton.h"
#include "ElaScrollPageArea.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToggleSwitch.h"
#include "ElaWindow.h"
#include "ElaLineEdit.h"
#include "ElaSlider.h"

#include "app_config.h"

T_Setting::T_Setting(QWidget* parent)
    : T_BasePage(parent)
{
    // 预览窗口标题
    ElaWindow* window = dynamic_cast<ElaWindow*>(parent);
    setWindowTitle("Setting");

    ElaText* themeText = new ElaText("主题设置", this);
    themeText->setWordWrap(false);
    themeText->setTextPixelSize(18);

    _themeComboBox = new ElaComboBox(this);
    _themeComboBox->addItem("日间模式");
    _themeComboBox->addItem("夜间模式");
    ElaScrollPageArea* themeSwitchArea = new ElaScrollPageArea(this);
    QHBoxLayout* themeSwitchLayout = new QHBoxLayout(themeSwitchArea);
    ElaText* themeSwitchText = new ElaText("主题切换", this);
    themeSwitchText->setWordWrap(false);
    themeSwitchText->setTextPixelSize(15);
    themeSwitchLayout->addWidget(themeSwitchText);
    themeSwitchLayout->addStretch();
    themeSwitchLayout->addWidget(_themeComboBox);
    connect(_themeComboBox, QOverload<int>::of(&ElaComboBox::currentIndexChanged), this, [=](int index) {
        if (index == 0)
        {
            eTheme->setThemeMode(ElaThemeType::Light);
        }
        else
        {
            eTheme->setThemeMode(ElaThemeType::Dark);
        }
    });
    connect(eTheme, &ElaTheme::themeModeChanged, this, [=](ElaThemeType::ThemeMode themeMode) {
        _themeComboBox->blockSignals(true);
        if (themeMode == ElaThemeType::Light)
        {
            _themeComboBox->setCurrentIndex(0);
        }
        else
        {
            _themeComboBox->setCurrentIndex(1);
        }
        _themeComboBox->blockSignals(false);
    });




    ElaText* helperText = new ElaText("应用程序设置", this);
    helperText->setWordWrap(false);
    helperText->setTextPixelSize(18);

    _micaSwitchButton = new ElaToggleSwitch(this);
    ElaScrollPageArea* micaSwitchArea = new ElaScrollPageArea(this);
    QHBoxLayout* micaSwitchLayout = new QHBoxLayout(micaSwitchArea);
    ElaText* micaSwitchText = new ElaText("启用云母效果(跨平台)", this);
    micaSwitchText->setWordWrap(false);
    micaSwitchText->setTextPixelSize(15);
    micaSwitchLayout->addWidget(micaSwitchText);
    micaSwitchLayout->addStretch();
    micaSwitchLayout->addWidget(_micaSwitchButton);
    connect(_micaSwitchButton, &ElaToggleSwitch::toggled, this, [=](bool checked) {
        eApp->setIsEnableMica(checked);
    });

    //ollama
    //提示词
    ElaLineEdit *promot_sentence_edit = new ElaLineEdit(this);
    promot_sentence_edit->setText("亲爱的，我是你的性感女友，我会为了你做任何事情。");
    AppConfig::instance().setPromotSentence(promot_sentence_edit->text());

    ElaScrollPageArea *promot_sentence_Area = new ElaScrollPageArea(this);
    QHBoxLayout* promot_sentence_Layout = new QHBoxLayout(promot_sentence_Area);
    ElaText *promot_sentence_Text = new ElaText("提示语句(记忆)", this);
    promot_sentence_Text->setWordWrap(false);
    promot_sentence_Text->setTextPixelSize(15);
    promot_sentence_Layout->addWidget(promot_sentence_Text);
    promot_sentence_Layout->addStretch();
    promot_sentence_Layout->addWidget(promot_sentence_edit);
    connect(promot_sentence_edit, &ElaLineEdit::textChanged, this, [=]() {
        //共用的全局设置数据
        AppConfig::instance().setPromotSentence(promot_sentence_edit->text());
        AppConfig::instance().saveSettings();
    });

    //token长度
    ElaSlider *tokenSize_slider = new ElaSlider(this);
    tokenSize_slider->setRange(10, 100);
    tokenSize_slider->setValue(AppConfig::instance().getTokenSize());
    ElaScrollPageArea *toke_size_Area = new ElaScrollPageArea(this);
    QHBoxLayout* toke_size_Layout = new QHBoxLayout(toke_size_Area);
    ElaText *toke_size_Text = new ElaText("句子生成长度", this);
    toke_size_Text->setWordWrap(false);
    toke_size_Text->setTextPixelSize(15);

    ElaText *toke_size_value = new ElaText(QString::number(tokenSize_slider->value()), this);
    toke_size_value->setWordWrap(false);
    toke_size_value->setTextPixelSize(15);

    toke_size_Layout->addWidget(toke_size_Text);
    toke_size_Layout->addStretch();
    toke_size_Layout->addWidget(tokenSize_slider);
    toke_size_Layout->addWidget(toke_size_value);
    connect(tokenSize_slider, &ElaSlider::valueChanged, this, [=]() {
        //共用的全局设置数据
        toke_size_value->setText(QString::number(tokenSize_slider->value()));
        AppConfig::instance().setTokenSize(tokenSize_slider->value());
        AppConfig::instance().saveSettings();
    });



    //tts
    _enableTTsButton = new ElaToggleSwitch(this);
    _enableTTsButton->setIsToggled(true);
    qDebug() << "启用 tts";
    ElaScrollPageArea *ttsSwitchArea = new ElaScrollPageArea(this);
    QHBoxLayout* ttsSwitchLayout = new QHBoxLayout(ttsSwitchArea);
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

    ttsCharacterSelect_combox->setCurrentIndex(AppConfig::instance().getTTsVoiceChoise()-1);

    ElaScrollPageArea *ttsCharacterSelect_Area = new ElaScrollPageArea(this);
    QHBoxLayout* ttsCharacterSelect_Layout = new QHBoxLayout(ttsCharacterSelect_Area);
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


    _logSwitchButton = new ElaToggleSwitch(this);
    ElaScrollPageArea* logSwitchArea = new ElaScrollPageArea(this);
    QHBoxLayout* logSwitchLayout = new QHBoxLayout(logSwitchArea);
    ElaText* logSwitchText = new ElaText("启用日志功能", this);
    logSwitchText->setWordWrap(false);
    logSwitchText->setTextPixelSize(15);
    logSwitchLayout->addWidget(logSwitchText);
    logSwitchLayout->addStretch();
    logSwitchLayout->addWidget(_logSwitchButton);
    connect(_logSwitchButton, &ElaToggleSwitch::toggled, this, [=](bool checked) {
        ElaLog::getInstance()->initMessageLog(checked);
        if (checked)
        {
            qDebug() << "日志已启用!";
        }
        else
        {
            qDebug() << "日志已关闭!";
        }
    });

    _minimumButton = new ElaRadioButton("Minimum", this);
    _compactButton = new ElaRadioButton("Compact", this);
    _maximumButton = new ElaRadioButton("Maximum", this);
    _autoButton = new ElaRadioButton("Auto", this);
    _autoButton->setChecked(true);
    ElaScrollPageArea* displayModeArea = new ElaScrollPageArea(this);
    QHBoxLayout* displayModeLayout = new QHBoxLayout(displayModeArea);
    ElaText* displayModeText = new ElaText("导航栏模式选择", this);
    displayModeText->setWordWrap(false);
    displayModeText->setTextPixelSize(15);
    displayModeLayout->addWidget(displayModeText);
    displayModeLayout->addStretch();
    displayModeLayout->addWidget(_minimumButton);
    displayModeLayout->addWidget(_compactButton);
    displayModeLayout->addWidget(_maximumButton);
    displayModeLayout->addWidget(_autoButton);
    connect(_minimumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
        {
            window->setNavigationBarDisplayMode(ElaNavigationType::Minimal);
        }
    });
    connect(_compactButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
        {
            window->setNavigationBarDisplayMode(ElaNavigationType::Compact);
        }
    });
    connect(_maximumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
        {
            window->setNavigationBarDisplayMode(ElaNavigationType::Maximal);
        }
    });
    connect(_autoButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
        {
            window->setNavigationBarDisplayMode(ElaNavigationType::Auto);
        }
    });

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Setting");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(themeText);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(themeSwitchArea);
    centerLayout->addSpacing(15);
    centerLayout->addWidget(helperText);
    centerLayout->addSpacing(10);

    centerLayout->addWidget(promot_sentence_Area);
    centerLayout->addWidget(toke_size_Area);
    centerLayout->addWidget(ttsSwitchArea);
    centerLayout->addWidget(ttsCharacterSelect_Area);

    centerLayout->addWidget(logSwitchArea);
    centerLayout->addWidget(micaSwitchArea);
    centerLayout->addWidget(displayModeArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);
}

T_Setting::~T_Setting()
{
}
