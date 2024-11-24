#include "T_Chat.h"


#include <QScrollBar>
#include <QVBoxLayout>

#include <QAudioFormat>
#include <QAudioOutput>
#include <QBuffer>
#include <QEventLoop>

#include <QAudioSink>
#include <QMediaPlayer>
#include <QTemporaryFile>


#include "ElaIcon.h"
#include "ElaIconButton.h"
#include "ElaImageCard.h"
#include "ElaInteractiveCard.h"
#include "ElaPopularCard.h"
#include "ElaPushButton.h"
#include "ElaScrollArea.h"
#include "ElaScrollPageArea.h"
#include "ElaText.h"

#include "app_config.h"
#include "history_storage.h"

T_Chat::T_Chat(QWidget *parent) : T_BasePage(parent)
{

    conversationTimes = AppConfig::instance().getConversationTimes();

    ElaImageCard *backgroundCard = new ElaImageCard(this);
    backgroundCard->setBorderRadius(10);
    backgroundCard->setFixedHeight(600);
    backgroundCard->setMaximumAspectRatio(1.7);

    QVBoxLayout *backgroundLayout = new QVBoxLayout(backgroundCard);
    backgroundLayout->setContentsMargins(0, 0, 0, 0);

    cardScrollArea = new ElaScrollArea(this);
    cardScrollArea->setWidgetResizable(true);
    cardScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setIsGrabGesture(true, 0);
    cardScrollArea->setIsOverShoot(Qt::Horizontal, true);

    cardScrollAreaWidget = new QWidget(this);
    cardScrollAreaWidget->setStyleSheet("background-color:transparent;");
    // cardScrollAreaWidget->setFixedHeight(400);
    cardScrollArea->setWidget(cardScrollAreaWidget);


    cardScrollAreaWidgetLayout = new QVBoxLayout(cardScrollAreaWidget);
    cardScrollAreaWidgetLayout->setContentsMargins(0, 0, 0, 0);
    cardScrollAreaWidgetLayout->addStretch();


    text_input_edit = new ElaLineEdit(this);
    text_input_edit->setFixedHeight(30);

    ElaPushButton *sendButton = new ElaPushButton(this);
    ElaPushButton *recevButton = new ElaPushButton(this);
    sendButton->setFixedHeight(30);
    // sendButton->set(ElaIconType::Car);

    QHBoxLayout *text_input_layout = new QHBoxLayout();
    text_input_layout->addWidget(recevButton);
    text_input_layout->addWidget(text_input_edit);
    text_input_layout->addWidget(sendButton);


    backgroundLayout->addWidget(cardScrollArea);
    backgroundLayout->addLayout(text_input_layout);


    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Chat");
    QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
    centerVLayout->setSpacing(0);
    centerVLayout->setContentsMargins(0, 0, 0, 0);
    centerVLayout->addWidget(backgroundCard);
    centerVLayout->addSpacing(20);


    this->addCentralWidget(centralWidget);

    connect(sendButton, &ElaPushButton::clicked, this, &T_Chat::inputTextEvent);
    // connect(recevButton, &ElaPushButton::clicked, this, &T_Chat::receiveTextEvent);
}

T_Chat::~T_Chat()
{
}

void T_Chat::inputTextEvent()
{
    if (text_input_edit->text().size() <= 0)
    {
        return;
    }
    qDebug() << "发送~";
    // ElaText *homeCard = new ElaText(cardScrollArea);

    // ElaScrollPageArea *textArea = new ElaScrollPageArea(this);
    // // textArea->setMaximumWidth(500);
    // // textArea->setB
    // textArea->setStyleSheet("background-color:transparent;");

    // ElaInteractiveCard *userCard = new ElaInteractiveCard(this);
    QLabel *userCard = new QLabel(this);

    // userCard->setCardPixMode(ElaCardPixType::PixMode::Ellipse);
    // userCard->setBaseSize(30, 30);
    // userCard->setCardPixmap(QPixmap(":/resources/image/640.png"));
    QPixmap image(":/resources/image/640.png");
    userCard->setFixedSize(50, 50);
    userCard->setPixmap(image.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    userCard->setStyleSheet("background-color:transparent;");

    ElaText *textText = new ElaText(text_input_edit->text(), this);
    textText->setMaximumWidth(300);
    textText->setWordWrap(true);
    textText->setTextPixelSize(15);
    textText->setAlignment(Qt::AlignLeft);
    textText->setStyleSheet("background-color: rgba(128, 128, 128, 20);"); // 设置半透明灰色背景

    QHBoxLayout *textLayout = new QHBoxLayout();
    textLayout->addStretch();
    textLayout->setAlignment(Qt::AlignRight);
    textLayout->addWidget(textText);
    textLayout->addWidget(userCard);
    // textLayout->addStretch();

    // textArea->setFixedHeight(50);
    // cardScrollAreaWidgetLayout->setAlignment(Qt::AlignRight);
    cardScrollAreaWidgetLayout->insertLayout(cardScrollAreaWidgetLayout->count() - 1, textLayout);

    // cardScrollAreaWidgetLayout->insertWidget(cardScrollAreaWidgetLayout->count() - 1, textArea);

    // cardScrollArea->verticalScrollBar()
    QScrollBar *vScrollBar = cardScrollArea->verticalScrollBar();
    vScrollBar->setValue(vScrollBar->maximum());

    // text_input_edit->clear();
    //保存当前文本指针
    current_sendText = textText;


    //然后发送到ollama
    send_requests_to_ollama();
}

void T_Chat::receiveTextEvent()
{
    if (text_input_edit->text().size() <= 0)
    {
        return;
    }
    qDebug() << "创建回复气泡";
    // ElaText *homeCard = new ElaText(cardScrollArea);

    // ElaScrollPageArea *textArea = new ElaScrollPageArea(this);
    // textArea->setStyleSheet("background-color:transparent;");
    // textArea->setMaximumWidth(500);

    ElaText *textText = new ElaText("", this);
    textText->setMaximumWidth(300);
    textText->setWordWrap(false);
    textText->setTextPixelSize(15);
    textText->setWordWrap(true);
    textText->setAlignment(Qt::AlignLeft);
    textText->setStyleSheet("background-color: rgba(128, 128, 128, 60);"); // 设置半透明灰色背景


    QVBoxLayout *textLayoutBase = new QVBoxLayout();
    QHBoxLayout *textLayout = new QHBoxLayout();

    ElaIconButton *audio_play_bt = new ElaIconButton(ElaIconType::Volume);
    audio_play_bt->setObjectName(QString::number(conversationTimes + 1));
    connect(audio_play_bt, &ElaIconButton::clicked, this, [=]() {
        QByteArray audioData;
        QString conversationIdx = audio_play_bt->objectName();
        history_storage::instance().readAudioData(conversationIdx, audioData);
        playAudio(audioData);
    });


    textLayout->setAlignment(Qt::AlignLeft);
    textLayout->addWidget(textText);

    textLayout->addStretch();

    textLayoutBase->addLayout(textLayout);
    textLayoutBase->addWidget(audio_play_bt);
    // textArea->setFixedHeight(50);
    // cardScrollAreaWidgetLayout->setAlignment(Qt::AlignRight);


    cardScrollAreaWidgetLayout->insertLayout(cardScrollAreaWidgetLayout->count() - 1, textLayoutBase);


    // cardScrollArea->verticalScrollBar()
    QScrollBar *vScrollBar = cardScrollArea->verticalScrollBar();
    vScrollBar->setValue(vScrollBar->maximum());

    //保存当前文本指针
    current_receiveText = textText;
}


void T_Chat::send_requests_to_tts()
{
    if (!AppConfig::instance().isEnableTTS())
    {
        return;
    }

    // QString currentString = plainTextEdit2->toPlainText();
    // if (received_txt==plainTextEdit2->toPlainText()){
    //     qDebug() << "未变化 播放旧音频";
    //     playAudio(response_data);
    //     return;
    // }

    qDebug() << "send to ollama!";
    ServeTTSRequest request;

    request.text = current_receiveText->text();
    qDebug() << "request.text " << request.text;
    // "合成所需的音频并流式返回";

    // request.references.append(ServeReferenceAudio("audio1", "text1"));
    // request.references.append(ServeReferenceAudio("audio2", "text2"));
    request.reference_id = "1";
    request.normalize = true;
    request.format = "wav";
    request.mp3_bitrate = 64;
    request.opus_bitrate = -1000;
    request.max_new_tokens = 0;
    request.chunk_length = 200;
    request.top_p = 0.7f;
    request.repetition_penalty = 1.2f;
    request.temperature = 0.7f;
    request.streaming = true;
    request.use_memory_cache = "never";
    request.seed = std::nullopt;
    ;

    QJsonObject requestJson = request.toJson();
    QJsonDocument requestJsonDoc(requestJson);
    // QByteArray requestData = doc.toJson();

    // qDebug() << requestData;


    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest netWorkRequest(QUrl("http://127.0.0.1:8080/v1/tts"));
    netWorkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    netWorkRequest.setRawHeader("authorization", "Bearer YOUR_API_KEY");


    QEventLoop loop; //使用 QEventLoop 阻塞 等待请求完成
    QNetworkReply *reply = manager->post(netWorkRequest, requestJsonDoc.toJson());

    qDebug() << "posted!";

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    response_data = reply->readAll();
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid())
    {
        int status = statusCode.toInt();
        qDebug() << "Status Code:" << status;
    }
    playAudio(response_data);

    // received_txt =

    reply->deleteLater();
}

void T_Chat::send_requests_to_tts_after_ollama_auto()
{
    ServeTTSRequest request;


    request.text = current_receiveText->text().remove("\\s");
    // "合成所需的音频并流式返回";

    // request.references.append(ServeReferenceAudio("audio1", "text1"));
    // request.references.append(ServeReferenceAudio("audio2", "text2"));
    request.reference_id = "3";
    request.normalize = true;
    request.format = "wav";
    request.mp3_bitrate = 64;
    request.opus_bitrate = -1000;
    request.max_new_tokens = 0;
    request.chunk_length = 200;
    request.top_p = 0.7f;
    request.repetition_penalty = 1.2f;
    request.temperature = 0.7f;
    request.streaming = true;
    request.use_memory_cache = "never";
    request.seed = std::nullopt;
    ;

    QJsonObject requestJson = request.toJson();
    QJsonDocument requestJsonDoc(requestJson);
    // QByteArray requestData = doc.toJson();

    // qDebug() << requestData;


    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest netWorkRequest(QUrl("http://127.0.0.1:8080/v1/tts"));
    netWorkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    netWorkRequest.setRawHeader("authorization", "Bearer YOUR_API_KEY");


    QEventLoop loop; //使用 QEventLoop 阻塞 等待请求完成
    QNetworkReply *reply = manager->post(netWorkRequest, requestJsonDoc.toJson());

    qDebug() << "posted!";

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    response_data = reply->readAll();
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid())
    {
        int status = statusCode.toInt();
        qDebug() << "Status Code:" << status;
    }
    qDebug() << "playAudio:";
    playAudio(response_data);

    //保存数据
    conversationTimes++;
    history_storage::instance().saveAudioData(response_data,
                                              QString::number(conversationTimes),
                                              current_receiveText->text());
    AppConfig::instance().setConversationTimes(conversationTimes);
    AppConfig::instance().saveSettings();
    // received_txt = plainTextEdit2->toPlainText();
    reply->deleteLater();
}

void T_Chat::send_requests_to_ollama()
{

    received_txt = "";
    qDebug() << "clicked!";
    OllamaRequest request;
    request.model = "qwen2-rp";
    request.stream = true;

    request.options.num_predict = AppConfig::instance().getTokenSize();

    Ollama_messages message_a{"assistant", AppConfig::instance().getPromotSentence()};
    // message_a.role = "assistant";
    // message_a.content = "亲爱的，我是你的性感女友，我会为了你做任何事情。";
    Ollama_messages message_u{"user", current_sendText->text()};
    // message_u.role = "user";
    // message_u.content = plainTextEdit1->toPlainText();

    request.messages.append(message_a);
    request.messages.append(message_u);


    QJsonObject requestJson = request.toJson();
    QJsonDocument requestJsonDoc(requestJson);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest netWorkRequest(QUrl("http://localhost:11434/api/chat"));
    netWorkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    netWorkRequest.setRawHeader("authorization", "Bearer YOUR_API_KEY");

    QEventLoop loop; //使用 QEventLoop 阻塞 等待请求完成
    QNetworkReply *reply = manager->post(netWorkRequest, requestJsonDoc.toJson());

    qDebug() << "posted!";

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    //

    // QByteArray response_data = reply->readAll();

    // qDebug() << response_data;
    receiveTextEvent();
    parseResponse(reply);

    loop.exec();

    if (!AppConfig::instance().isEnableTTS())
    {
        return;
    }
    qDebug() << "收到回复 自动转tts。。。";
    send_requests_to_tts_after_ollama_auto();
}


void T_Chat::parseResponse(QNetworkReply *reply)
{

    receiving_txt = "";
    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
        static QByteArray buffer;
        buffer.append(reply->readAll());

        while (true)
        {
            int endIndex = buffer.indexOf('\n');
            if (endIndex == -1)
            {
                break;
            }

            QByteArray jsonObjectData = buffer.left(endIndex).trimmed();
            buffer.remove(0, endIndex + 1);

            if (jsonObjectData.isEmpty())
            {
                continue;
            }

            QJsonDocument jsonResponse = QJsonDocument::fromJson(jsonObjectData);
            if (!jsonResponse.isNull() && jsonResponse.isObject())
            {
                QJsonObject jsonObject = jsonResponse.object();
                if (jsonObject.contains("message") && jsonObject["message"].isObject())
                {
                    QJsonObject messageObject = jsonObject["message"].toObject();
                    if (messageObject.contains("content") && messageObject["content"].isString())
                    {
                        QString content = messageObject["content"].toString();
                        receiving_txt += content;
                        // qDebug() << "Parsed content:" << receiving_txt;
                        current_receiveText->clear();
                        qDebug() << current_receiveText;
                        current_receiveText->setText(receiving_txt.remove("\\s"));

                        // 在这里可以使用content变量，例如更新UI
                        // ui->text_talk->setText(ui->text_talk->toPlainText() + content);
                    }
                }
            }
            else
            {
                qDebug() << "Failed to parse JSON object:" << jsonObjectData;
            }
        }

        if (reply->atEnd())
        {
            qDebug() << "***********************done";
            // 处理完成后的逻辑
            // ui->start_talk->setEnabled(true);
            // emit startClicked();
        }
    });
}

void T_Chat::playAudio(const QByteArray &audioData)
{
    if(isPlayingAudio && audio){
        qDebug() << "正在播放 停止播放";
        audio->stop();
        delete audio;
        audio = nullptr;
        isPlayingAudio = false;
    }
    qDebug() << "in playAudio ";
    // QMediaPlayer * player = new QMediaPlayer(this);
    // QAudioOutput * audioouput = new QAudioOutput(this);
    // audioouput->setVolume(40);

    // player->setAudioOutput(audioouput);

    // QTemporaryFile *tempFile = new QTemporaryFile(this);
    // if (tempFile->open()) {
    //     tempFile->write(audioData);
    //     tempFile->flush();
    //     tempFile->close();

    //     player->setSource(QUrl::fromLocalFile(tempFile->fileName()));

    //     player->play();
    // }


    // if (tempFile->open())
    // {
    //     tempFile->write(audioData);
    //     tempFile->flush();
    //     tempFile->close();
    // }

    QBuffer *audioBuffer = new QBuffer(this);
    audioBuffer->setData(audioData);
    audioBuffer->open(QIODevice::ReadOnly);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);

    format.setSampleFormat(QAudioFormat::SampleFormat::Int16);

    audio = new QAudioSink(format, this);


    qDebug() << "start";
    audio->start(audioBuffer);
    isPlayingAudio = true;
    connect(audio, &QAudioSink::stateChanged, this, [=](QAudio::State state) {
            if (state == QAudio::StoppedState)
            {
                isPlayingAudio = false;
                audioBuffer->deleteLater();
                audio->deleteLater();
                audio = nullptr;
            }
        });
}
