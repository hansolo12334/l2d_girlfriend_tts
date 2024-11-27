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

#include  "T_ChatBubble.h"

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

    connect(text_input_edit, &ElaLineEdit::returnPressed, this, &T_Chat::inputTextEvent);

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
    APP_LOG_DEBUG("发送~");
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

    // ElaText *textText = new ElaText(text_input_edit->text(), this);
    // textText->setMaximumWidth(300);
    // textText->setWordWrap(false);
    // textText->setTextPixelSize(15);
    // textText->setAlignment(Qt::AlignLeft);
    // textText->setStyleSheet("background-color: rgba(128, 128, 128, 20);"); // 设置半透明灰色背景

    //测试气泡
    T_ChatBubble *chatBubble = new T_ChatBubble();
    chatBubble->setText(text_input_edit->text());
    chatBubble->setTextPixelSize(15);
    chatBubble->setMaximumWidth(300);
    chatBubble->setWordWrap(false);
    // chatBubble->resize(60, 30);

    QHBoxLayout *textLayout = new QHBoxLayout();
    textLayout->addStretch();
    textLayout->setAlignment(Qt::AlignRight);



    textLayout->addWidget(chatBubble);
    textLayout->addWidget(userCard);
    // textLayout->addStretch();

    // textArea->setFixedHeight(50);
    // cardScrollAreaWidgetLayout->setAlignment(Qt::AlignRight);
    cardScrollAreaWidgetLayout->insertLayout(cardScrollAreaWidgetLayout->count() - 1, textLayout);

    // cardScrollAreaWidgetLayout->insertWidget(cardScrollAreaWidgetLayout->count() - 1, textArea);

    // cardScrollArea->verticalScrollBar()
    QCoreApplication::processEvents();
    QScrollBar *vScrollBar = cardScrollArea->verticalScrollBar();
    vScrollBar->setValue(vScrollBar->maximum());


    text_input_edit->clear();
    //保存当前文本指针
    current_sendText = chatBubble;


    //然后发送到ollama
    bool send_state=send_requests_to_ollama();
}

void T_Chat::receiveTextEvent()
{
    if (current_sendText->text().size() <= 0)
    {
        return;
    }
    APP_LOG_DEBUG("创建回复气泡");
    // ElaText *homeCard = new ElaText(cardScrollArea);

    // ElaScrollPageArea *textArea = new ElaScrollPageArea(this);
    // textArea->setStyleSheet("background-color:transparent;");
    // textArea->setMaximumWidth(500);

    // ElaText *textText = new ElaText("", this);
    // textText->setMaximumWidth(300);
    // textText->setWordWrap(false);
    // textText->setTextPixelSize(15);
    // textText->setWordWrap(true);
    // textText->setAlignment(Qt::AlignLeft);
    // textText->setStyleSheet("background-color: rgba(128, 128, 128, 60);"); // 设置半透明灰色背景

    //测试气泡
    T_ChatBubble *chatBubble = new T_ChatBubble();
    chatBubble->setText("");
    chatBubble->setTextPixelSize(15);
    chatBubble->setMaxWidth(200);
    // chatBubble->setWordWrap(false);


    QVBoxLayout *textLayoutBase = new QVBoxLayout();
    QHBoxLayout *textLayout = new QHBoxLayout();
    QHBoxLayout *toolsLayout=new QHBoxLayout();

    ElaIconButton *audio_play_bt = new ElaIconButton(ElaIconType::Volume);

    audio_play_bt->setObjectName(QString::number(conversationTimes + 1));
    connect(audio_play_bt, &ElaIconButton::clicked, this, [=]() {
        QByteArray audioData;
        QString conversationIdx = audio_play_bt->objectName();
        bool re=history_storage::instance().readAudioData(conversationIdx, audioData);
        if(re){
            playAudio(audioData);
        }

    });

    ElaIconButton *audio_recreate_bt = new ElaIconButton(ElaIconType::RotateReverse);
    audio_recreate_bt->setObjectName("audioRecreatBt_"+QString::number(conversationTimes + 1));
    connect(audio_recreate_bt, &ElaIconButton::clicked, this, [=]() {
        QString conversationIdx = audio_recreate_bt->objectName();
        QStringList temp = conversationIdx.split("_");
        QString data;
        APP_LOG_DEBUG(temp[1]);
        bool re = history_storage::instance().readConversationString(temp[1], data);
        if(re){
            APP_LOG_DEBUG("重新生成当前回复内容tts: " << data);
            send_requests_to_tts(data);
        }

    });

    textLayout->setAlignment(Qt::AlignLeft);
    textLayout->addWidget(chatBubble);

    textLayout->addStretch();

    toolsLayout->addWidget(audio_recreate_bt);
    toolsLayout->addWidget(audio_play_bt);
    toolsLayout->addStretch();


    textLayoutBase->addLayout(textLayout);
    textLayoutBase->addLayout(toolsLayout);

    // textArea->setFixedHeight(50);
    // cardScrollAreaWidgetLayout->setAlignment(Qt::AlignRight);


    cardScrollAreaWidgetLayout->insertLayout(cardScrollAreaWidgetLayout->count() - 1, textLayoutBase);

    QCoreApplication::processEvents();
    // cardScrollArea->verticalScrollBar()
    QScrollBar *vScrollBar = cardScrollArea->verticalScrollBar();
    vScrollBar->setValue(vScrollBar->maximum());

    //保存当前文本指针
    current_receiveText = chatBubble;
}


void T_Chat::send_requests_to_tts(QString data)
{
    if (!AppConfig::instance().isEnableTTS())
    {
        return;
    }



    APP_LOG_DEBUG("send to tts!");
    ServeTTSRequest request;

    request.text = data.remove("\\r").remove("\\n");
    // qDebug() << "request.text " << request.text;
    // "合成所需的音频并流式返回";

    // request.references.append(ServeReferenceAudio("audio1", "text1"));
    // request.references.append(ServeReferenceAudio("audio2", "text2"));
    request.reference_id = QString::number(AppConfig::instance().getTTsVoiceChoise());
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


    APP_LOG_DEBUG("posted!");

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    response_data = reply->readAll();
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid())
    {
        int status = statusCode.toInt();
        APP_LOG_DEBUG("Status Code:" << status);
    }
    playAudio(response_data);

    // received_txt =

    reply->deleteLater();
}

bool T_Chat::send_requests_to_tts_after_ollama_auto()
{

    if (!AppConfig::instance().isEnableTTS())
    {
        //无音频保存当前对话
        APP_LOG_DEBUG("无音频保存当前对话 " << conversationTimes);
        conversationTimes++;
        history_storage::instance().saveConversationData(response_data,
                                                    QString::number(conversationTimes),
                                                    current_receiveText->text(),
                                                    current_sendText->text(),
                                                    false);
        AppConfig::instance().setConversationTimes(conversationTimes);
        AppConfig::instance().saveSettings();
        return true;
    }

    ServeTTSRequest request;


    request.text = current_receiveText->text().remove("\\r").remove("\\n");
    // "合成所需的音频并流式返回";
    APP_LOG_DEBUG("request.text " << request.text);
    // request.references.append(ServeReferenceAudio("audio1", "text1"));
    // request.references.append(ServeReferenceAudio("audio2", "text2"));
    request.reference_id = QString::number(AppConfig::instance().getTTsVoiceChoise());
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


    APP_LOG_DEBUG("posted");

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    response_data = reply->readAll();
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid())
    {
        int status = statusCode.toInt();
        APP_LOG_DEBUG("Status Code:" << status);
    }else{
        APP_LOG_ERROR("未收到tts数据");
        return false;
    }
    APP_LOG_DEBUG("playAudio...");
    playAudio(response_data);

    //保存数据
    conversationTimes++;
    history_storage::instance().saveConversationData(response_data,
                                              QString::number(conversationTimes),
                                              current_receiveText->text(),
                                              current_sendText->text(),
                                              true);
    AppConfig::instance().setConversationTimes(conversationTimes);
    AppConfig::instance().saveSettings();
    // received_txt = plainTextEdit2->toPlainText();
    reply->deleteLater();

    return true;
}

bool T_Chat::send_requests_to_ollama()
{

    received_txt = "";

    APP_LOG_DEBUG("clicked!");
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


    APP_LOG_DEBUG("posted!");

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    //

    // QByteArray response_data = reply->readAll();

    // qDebug() << response_data;
    receiveTextEvent();
    parseResponse(reply);

    loop.exec();
    current_receiveText->setText(receiving_txt.remove("\\r").remove("\n"));

    if (!AppConfig::instance().isEnableTTS())
    {
        return true;
    }
    if(receiving_txt.size()<=1){
        APP_LOG_ERROR("未成功收到ollam消息");
        current_receiveText->setText("未连接。。。");
        return false;
    }

    APP_LOG_DEBUG("收到回复 自动转tts...");

    if(!send_requests_to_tts_after_ollama_auto()){
        return false;
    }

    return true;
}


void T_Chat::parseResponse(QNetworkReply *reply)
{

    receiving_txt = "";
    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
        // static QByteArray buffer;
        // buffer.append(reply->readAll());
        // QJsonDocument jsonResponse = QJsonDocument::fromJson(buffer);
        // if (!jsonResponse.isNull() && jsonResponse.isObject())
        // {
        //     QJsonObject jsonObject = jsonResponse.object();
        //     if (jsonObject.contains("message") && jsonObject["message"].isObject())
        //     {
        //         QJsonObject messageObject = jsonObject["message"].toObject();
        //         if (messageObject.contains("content") && messageObject["content"].isString())
        //         {
        //             QString content = messageObject["content"].toString();
        //             receiving_txt += content;
        //             current_receiveText->clear();
        //             // current_receiveText->setText(receiving_txt.remove("\\r").remove("\n"));
        //         }
        //     }
        // }

        //旧方法
        static QByteArray buffer;
        buffer.append(reply->readAll());
        while (true)
        {

            //旧方法
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
                        // current_receiveText->clear();
                        // qDebug() << current_receiveText;
                        // current_receiveText->setText(receiving_txt.remove("\\r").remove("\n"));

                        // 在这里可以使用content变量，例如更新UI
                        // ui->text_talk->setText(ui->text_talk->toPlainText() + content);
                    }
                }
            }
            else
            {
                APP_LOG_DEBUG("Failed to parse JSON object:" << jsonObjectData);
            }
        }

        if (reply->atEnd())
        {
            APP_LOG_DEBUG("->>>>receivedone<<<<--");
            // 处理完成后的逻辑
            // ui->start_talk->setEnabled(true);
            // emit startClicked();
        }
    });
}

void T_Chat::playAudio(const QByteArray &audioData)
{
    if(isPlayingAudio && audio){
        APP_LOG_DEBUG("正在播放 停止播放");
        audio->stop();
        delete audio;
        audio = nullptr;
        isPlayingAudio = false;
    }
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



    APP_LOG_DEBUG("start play");
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
