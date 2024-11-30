#include"dialog_input_edit.h"
#include <QApplication>
#include <QGuiApplication>
#include <Windows.h>
#include <dwmapi.h>

#include "ElaTheme.h"

#include "app_log.h"

#include"ollama_api.h"
#include "tts_api.h"

#include "app_config.h"

#include "media_manager.h"

#include "ElaSlider.h"

#include"LAppLive2DManager.hpp"
#include"LAppModel.hpp"
#include <CubismDefaultParameterId.hpp>


dialogInputEdit::dialogInputEdit(QWidget *parent)
 :QWidget(parent)
{
    auto viewId = this->winId();
    DWM_BLURBEHIND bb = { 0 };
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow((HWND)viewId, &bb);

    _inputLineEdt = new QLineEdit(this);
    _inputLineEdt->setFixedHeight(60);
    _inputLineEdt->setFixedWidth(180);
    // _inputLineEdt->setBorderRadius(20);
    _inputLineEdt->setStyleSheet("border: 30px ;");
    QFont font;
    font.setPixelSize(15);
    _inputLineEdt->setFont(font);
    connect(_inputLineEdt, &QLineEdit::returnPressed, this, &dialogInputEdit::inputTextEvent);

    _sendChatBt = new ElaIconButton(ElaIconType::ArrowUpFromArc);
    _sendChatBt->setFixedHeight(60);
    _sendChatBt->setFixedWidth(60);
    _sendChatBt->setLightHoverColor(ElaThemeColor(ElaThemeType::Light, PrimaryHover));

    _closeCurrentBt = new ElaIconButton(ElaIconType::Xmark);
    _closeCurrentBt->setFixedHeight(60);
    _closeCurrentBt->setFixedWidth(60);
    _closeCurrentBt->setLightHoverColor(QColor(0xE8, 0x11, 0x23));
    _closeCurrentBt->autoFillBackground();
    connect(_closeCurrentBt, &ElaIconButton::clicked, this, [=]() { this->hide(); });

    _voiceInputBt = new ElaIconButton(ElaIconType::Phone);
    _voiceInputBt->setFixedHeight(60);
    _voiceInputBt->setFixedWidth(60);
    _voiceInputBt->setLightHoverColor(ElaThemeColor(ElaThemeType::Light, PrimaryHover));
    _voiceInputBt->autoFillBackground();


    ElaSlider *tokenSize_slider = new ElaSlider(this);
    tokenSize_slider->setRange(0, 10);
    connect(tokenSize_slider, &ElaSlider::valueChanged, this, [=]() {
        //共用的全局设置数据
        // qDebug() << tokenSize_slider->value();
        Live2D::Cubism::Framework::csmFloat32 vv = static_cast<Live2D::Cubism::Framework::csmFloat32>(tokenSize_slider->value() * 0.1f);
        LAppLive2DManager::GetInstance()->user_lipSync(vv);
    });

    _mainLayout = new QHBoxLayout();
    _mainLayout->setContentsMargins(0, 0, 0, 0);
    _mainLayout->addWidget(_voiceInputBt);
    _mainLayout->addWidget(_inputLineEdt);
    _mainLayout->addWidget(_sendChatBt);
    _mainLayout->addWidget(_closeCurrentBt);
    _mainLayout->addWidget(tokenSize_slider);


    this->setLayout(_mainLayout);
    this->resize(500, 100);

    // this->setAttribute(Qt::WA_NoSystemBackground, true);
    // this->setAttribute(Qt::WA_TranslucentBackground,true);

    this->setStyleSheet("border: 50px; background-color:rgba(249, 246, 246, 190);");
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowType::MSWindowsOwnDC,false);
    this->setWindowFlag(Qt::Tool);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);


    this->setMouseTracking(true);
}


void dialogInputEdit::moveToButtom()
{
    QScreen *p_screen = QGuiApplication::primaryScreen();
    int cxScreen,cyScreen;
    cxScreen=GetSystemMetrics(SM_CXSCREEN);
    cyScreen=GetSystemMetrics(SM_CYSCREEN);
    qreal dpiScale = p_screen->devicePixelRatio();
    auto realWindowWidth = cxScreen / dpiScale;
    auto realWindowHeight = cyScreen / dpiScale;

    auto moveto_x = realWindowWidth / 2 - this->window()->x() / 2;
    auto moveto_y = realWindowHeight  -350;
    this->move(moveto_x, moveto_y);
}


void dialogInputEdit::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        APP_LOG_DEBUG("LeftButtonPress");
        m_dragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}
void dialogInputEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}
void dialogInputEdit::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        APP_LOG_DEBUG("LeftButtonRelease");
        m_dragging = false;
        event->accept();
    }
}


void dialogInputEdit::inputTextEvent()
{
    // if (_inputLineEdt->text().isEmpty()){
    //     return;
    // }

    // Ollama::OllamaRequest ollama_request;
    // ollama_request.model = "qwen2-rp";
    // ollama_request.stream = true;

    // ollama_request.options.num_predict = AppConfig::instance().getTokenSize();

    // Ollama::Ollama_messages message_a{"assistant", AppConfig::instance().getPromotSentence()};
    // // message_a.role = "assistant";
    // // message_a.content = "亲爱的，我是你的性感女友，我会为了你做任何事情。";
    // Ollama::Ollama_messages message_u{"user", _inputLineEdt->text()};
    // // message_u.role = "user";
    // // message_u.content = plainTextEdit1->toPlainText();

    // ollama_request.messages.append(message_a);
    // ollama_request.messages.append(message_u);


    // QString re_message = "";
    // bool re = Ollama::OllamaAPI::instance().send_message_to_server(ollama_request, re_message);

    // if(re){
    //     APP_LOG_DEBUG(re_message);
    // }

    // tts
    TTS::ServeTTSRequest tts_request;

    // tts_request.text = re_message.remove("\\r").remove("\\n");
    tts_request.text = _inputLineEdt->text().remove("\\r").remove("\\n");
    // "合成所需的音频并流式返回";
    APP_LOG_DEBUG("request.text " << tts_request.text);
    // request.references.append(ServeReferenceAudio("audio1", "text1"));
    // request.references.append(ServeReferenceAudio("audio2", "text2"));
    tts_request.reference_id = QString::number(AppConfig::instance().getTTsVoiceChoise());
    tts_request.normalize = true;
    tts_request.format = "wav";
    tts_request.mp3_bitrate = 64;
    tts_request.opus_bitrate = -1000;
    tts_request.max_new_tokens = 0;
    tts_request.chunk_length = 200;
    tts_request.top_p = 0.7f;
    tts_request.repetition_penalty = 1.2f;
    tts_request.temperature = 0.7f;
    tts_request.streaming = true;
    tts_request.use_memory_cache = "never";
    tts_request.seed = std::nullopt;

    QByteArray response_data;
    bool tts_re=TTS::TTSAPI::instance().send_message_to_server(tts_request, response_data);

    if(tts_re){
        // AudioHandler::instance().playAudio(response_data);
        AudioHandler::instance().playAudio_pull(response_data);
        connect(&AudioHandler::instance(), &AudioHandler::playAudioRms, this, [&](double value) {
            Live2D::Cubism::Framework::csmFloat32 vv = static_cast<Live2D::Cubism::Framework::csmFloat32>(value);
            LAppLive2DManager::GetInstance()->user_lipSync(vv);

        });
    }
}
