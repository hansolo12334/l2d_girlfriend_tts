#include "T_Ollama_Setting.h"

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

T_Ollama_Setting::T_Ollama_Setting(QWidget *parent) : T_BasePage(parent)
{


    ollama_progress = new QProcess(this);

    ElaWindow *window = dynamic_cast<ElaWindow *>(parent);
    setWindowTitle("Ollama Setting");

    //ollama
    //启动
    ElaToggleSwitch *enable_ollama_switch = new ElaToggleSwitch(this);
    enable_ollama_switch->setIsToggled(false);
    connect(enable_ollama_switch, &ElaToggleSwitch::toggled, this, [=](bool checked) { open_ollama(checked); });
    //
    ElaScrollPageArea *ollama_start_Area = new ElaScrollPageArea(this);
    QHBoxLayout *ollama_start_Layout = new QHBoxLayout(ollama_start_Area);
    ElaText *ollama_start_Text = new ElaText("ollma启动", this);
    ollama_start_Text->setWordWrap(true);
    ollama_start_Text->setTextPixelSize(15);
    ollama_start_Layout->addWidget(ollama_start_Text);
    ollama_start_Layout->addStretch();
    ollama_start_Layout->addWidget(enable_ollama_switch);


    //提示词
    ElaLineEdit *promot_sentence_edit = new ElaLineEdit(this);
    promot_sentence_edit->setText("亲爱的，我是你的性感女友，我会为了你做任何事情。");
    promot_sentence_edit->setMaximumWidth(500);
    AppConfig::instance().setPromotSentence(promot_sentence_edit->text());

    ElaScrollPageArea *promot_sentence_Area = new ElaScrollPageArea(this);
    QHBoxLayout *promot_sentence_Layout = new QHBoxLayout(promot_sentence_Area);
    ElaText *promot_sentence_Text = new ElaText("提示语句(记忆)", this);
    promot_sentence_Text->setWordWrap(true);
    promot_sentence_Text->setTextPixelSize(15);
    promot_sentence_Layout->addWidget(promot_sentence_Text);
    // promot_sentence_Layout->addStretch();
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
    QHBoxLayout *toke_size_Layout = new QHBoxLayout(toke_size_Area);
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




    ElaLineEdit *api_address_edit = new ElaLineEdit(this);
    api_address_edit->setText("http://localhost:11434/api/chat");
    api_address_edit->setMaximumWidth(500);
    AppConfig::instance().setOllamaApiAddress(api_address_edit->text());

    ElaScrollPageArea *api_address_Area = new ElaScrollPageArea(this);
    QHBoxLayout *api_address_Layout = new QHBoxLayout(api_address_Area);
    ElaText *api_address_Text = new ElaText("api地址", this);
    api_address_Text->setWordWrap(true);
    api_address_Text->setTextPixelSize(15);
    api_address_Layout->addWidget(api_address_Text);
    // promot_sentence_Layout->addStretch();
    api_address_Layout->addWidget(api_address_edit);
    connect(api_address_edit, &ElaLineEdit::textChanged, this, [=]() {
        //共用的全局设置数据
        AppConfig::instance().setOllamaApiAddress(api_address_edit->text());
        AppConfig::instance().saveSettings();
    });




    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Ollama对话模型设置");
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(api_address_Area);
    centerLayout->addWidget(ollama_start_Area);
    centerLayout->addWidget(promot_sentence_Area);
    centerLayout->addWidget(toke_size_Area);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    addCentralWidget(centralWidget, true, true, 0);

}


T_Ollama_Setting::~T_Ollama_Setting()
{

}

void T_Ollama_Setting::open_ollama(bool enable)
{
    if (enable)
    {

        QString ollam_bat_path = "D:\\Qt_project\\2024\\l2d_girlfriend_tts\\start_ollama.bat";
        APP_LOG_DEBUG("启动ollama: bat path " << ollam_bat_path);
        // QString ollam_bat_path = "D:\\Qt_project\\2024\\l2d_girlfriend_tts\\start_ollama.bat";
        ollama_progress->start("cmd.exe", {"/c", "start", ollam_bat_path});
    }
    else
    {
        APP_LOG_DEBUG("关闭ollama: ");
        // tts_progress->terminate();
    }
}
