#include "T_Home.h"

#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

#include <QAudioFormat>
#include <QAudioOutput>
#include <QBuffer>
#include <QEventLoop>

#include <QMediaPlayer>

#include <QAudioSink>
#include <QTemporaryFile>

#include "ElaAcrylicUrlCard.h"
#include "ElaFlowLayout.h"
#include "ElaImageCard.h"
#include "ElaMenu.h"
#include "ElaMessageBar.h"
#include "ElaNavigationRouter.h"
#include "ElaPopularCard.h"
#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaToolTip.h"

#include "ElaLineEdit.h"
#include "ElaPlainTextEdit.h"

#include "ElaPushButton.h"

#include <QDebug>

#include"app_config.h"

T_Home::T_Home(QWidget *parent) : T_BasePage(parent)
{
    tempFile = new QTemporaryFile(this);

    // 预览窗口标题
    setWindowTitle("Home");

    setTitleVisible(false);
    setContentsMargins(2, 2, 0, 0);
    // 标题卡片区域
    ElaText *desText = new ElaText("FluentUI For QWidget", this);
    desText->setTextPixelSize(18);
    ElaText *titleText = new ElaText("ElaWidgetTools", this);
    titleText->setTextPixelSize(35);

    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setContentsMargins(30, 60, 0, 0);
    titleLayout->addWidget(desText);
    titleLayout->addWidget(titleText);

    ElaImageCard *backgroundCard = new ElaImageCard(this);
    backgroundCard->setBorderRadius(10);
    backgroundCard->setFixedHeight(600);
    backgroundCard->setMaximumAspectRatio(1.7);
    // backgroundCard->setCardImage(QImage(":/resources/image/Home_Background.png"));

    // ElaAcrylicUrlCard* urlCard1 = new ElaAcrylicUrlCard(this);
    // urlCard1->setCardPixmapSize(QSize(62, 62));
    // urlCard1->setFixedSize(195, 225);
    // urlCard1->setTitlePixelSize(17);
    // urlCard1->setTitleSpacing(25);
    // urlCard1->setSubTitleSpacing(13);
    // urlCard1->setUrl("https://github.com/Liniyous/ElaWidgetTools");
    // urlCard1->setCardPixmap(QPixmap(":/Resource/Image/github.png"));
    // urlCard1->setTitle("ElaTool Github");
    // urlCard1->setSubTitle("Use ElaWidgetTools To Create A Cool Project");
    // ElaToolTip* urlCard1ToolTip = new ElaToolTip(urlCard1);
    // urlCard1ToolTip->setToolTip("https://github.com/Liniyous/ElaWidgetTools");
    // ElaAcrylicUrlCard* urlCard2 = new ElaAcrylicUrlCard(this);
    // urlCard2->setCardPixmapSize(QSize(62, 62));
    // urlCard2->setFixedSize(195, 225);
    // urlCard2->setTitlePixelSize(17);
    // urlCard2->setTitleSpacing(25);
    // urlCard2->setSubTitleSpacing(13);
    // urlCard2->setUrl("https://space.bilibili.com/21256707");
    // urlCard2->setCardPixmap(QPixmap(":/Resource/Image/Moon.jpg"));
    // urlCard2->setTitle("ElaWidgetTool");
    // urlCard2->setSubTitle("8009963@qq.com");

    // ElaScrollArea* cardScrollArea = new ElaScrollArea(this);
    // cardScrollArea->setWidgetResizable(true);
    // cardScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // cardScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // cardScrollArea->setIsGrabGesture(true, 0);
    // cardScrollArea->setIsOverShoot(Qt::Horizontal, true);
    // QWidget* cardScrollAreaWidget = new QWidget(this);
    // cardScrollAreaWidget->setStyleSheet("background-color:transparent;");
    // cardScrollArea->setWidget(cardScrollAreaWidget);
    // QHBoxLayout* urlCardLayout = new QHBoxLayout();
    // urlCardLayout->setSpacing(15);
    // urlCardLayout->setContentsMargins(30, 0, 0, 6);
    // urlCardLayout->addWidget(urlCard1);
    // urlCardLayout->addWidget(urlCard2);
    // urlCardLayout->addStretch();
    // QVBoxLayout* cardScrollAreaWidgetLayout = new QVBoxLayout(cardScrollAreaWidget);
    // cardScrollAreaWidgetLayout->setContentsMargins(0, 0, 0, 0);
    // cardScrollAreaWidgetLayout->addStretch();
    // cardScrollAreaWidgetLayout->addLayout(urlCardLayout);



    textEdit1 = new ElaLineEdit(this);
    // textEdit1->setPlaceholderText(QString("亲爱的，我是你的性感女友，我会为了你做任何事情。"));
    textEdit1->setText(QString("亲爱的，我是你的温柔女友，我会为了你做任何事情。"));

    promot_size_Edit1 = new ElaLineEdit(this);
    promot_size_Edit1->setText("50");

    plainTextEdit1 = new ElaPlainTextEdit(this);
    plainTextEdit1->setPlainText("你是谁？");


    plainTextEdit2 = new ElaPlainTextEdit(this);
    plainTextEdit2->setPlainText("测试文字responce");

    combx_voice = new ElaComboBox(this);
    QStringList combx{"1", "2", "3"};
    combx_voice->addItems(combx);
    ElaText *voiceText = new ElaText("音色选择", this);
    voiceText->setTextPixelSize(20);
    QHBoxLayout *voiceLayout = new QHBoxLayout();
    voiceLayout->addWidget(voiceText);
    voiceLayout->addWidget(combx_voice);

    ElaText *promot_size_Text = new ElaText("句子长度", this);
    promot_size_Text->setTextPixelSize(20);
    QHBoxLayout *promot_setting_layout=new QHBoxLayout();
    promot_setting_layout->addWidget(promot_size_Text);
    promot_setting_layout->addWidget(promot_size_Edit1);


    ElaPushButton *pushbt1 = new ElaPushButton("发送request到tts", this);
    ElaPushButton *pushbt2 = new ElaPushButton("发送request到Ollama", this);
    connect(pushbt1, SIGNAL(clicked()), this, SLOT(send_requests()));
    connect(pushbt2, SIGNAL(clicked()), this, SLOT(send_requests_to_ollama()));

    QVBoxLayout *backgroundLayout = new QVBoxLayout(backgroundCard);
    // QVBoxLayout *backgroundLayout = new QVBoxLayout(this);
    backgroundLayout->setContentsMargins(0, 0, 0, 0);




    ElaScrollArea* cardScrollArea = new ElaScrollArea(this);
    cardScrollArea->setWidgetResizable(true);
    cardScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea->setIsGrabGesture(true, 0);
    cardScrollArea->setIsOverShoot(Qt::Horizontal, true);
    QWidget* cardScrollAreaWidget = new QWidget(this);
    cardScrollAreaWidget->setStyleSheet("background-color:transparent;");
    cardScrollAreaWidget->setFixedHeight(400);
    cardScrollArea->setWidget(cardScrollAreaWidget);

    QVBoxLayout* cardScrollAreaWidgetLayout = new QVBoxLayout(cardScrollAreaWidget);
    cardScrollAreaWidgetLayout->setContentsMargins(0, 0, 0, 0);
    cardScrollAreaWidgetLayout->addStretch();



    cardScrollAreaWidgetLayout->addWidget(textEdit1);

    cardScrollAreaWidgetLayout->addWidget(plainTextEdit1);
    cardScrollAreaWidgetLayout->addWidget(plainTextEdit2);

    cardScrollAreaWidgetLayout->addLayout(voiceLayout);
    cardScrollAreaWidgetLayout->addLayout(promot_setting_layout);

    cardScrollAreaWidgetLayout->addWidget(pushbt1);
    cardScrollAreaWidgetLayout->addWidget(pushbt2);

    backgroundLayout->addLayout(titleLayout);
    backgroundLayout->addWidget(cardScrollArea);
    backgroundLayout->addStretch();
    // // 推荐卡片
    // ElaText* flowText = new ElaText("热门免费应用", this);
    // flowText->setTextPixelSize(20);
    // QHBoxLayout* flowTextLayout = new QHBoxLayout();
    // flowTextLayout->setContentsMargins(33, 0, 0, 0);
    // flowTextLayout->addWidget(flowText);
    // // ElaFlowLayout
    // ElaPopularCard* homeCard = new ElaPopularCard(this);
    // connect(homeCard, &ElaPopularCard::popularCardButtonClicked, this, [=]() {
    //     QDesktopServices::openUrl(QUrl("https://github.com/Liniyous/ElaWidgetTools"));
    // });
    // homeCard->setCardPixmap(QPixmap(":/Resource/Image/Cirno.jpg"));
    // homeCard->setTitle("ElaWidgetTool");
    // homeCard->setSubTitle("5.0⭐ 实用程序与工具");
    // homeCard->setInteractiveTips("免费下载");
    // homeCard->setDetailedText("ElaWidgetTools致力于为QWidget用户提供一站式的外观和实用功能解决方案,只需数十MB内存和极少CPU占用以支持高效而美观的界面开发");
    // homeCard->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    // ElaPopularCard* homeCard1 = new ElaPopularCard(this);
    // connect(homeCard1, &ElaPopularCard::popularCardButtonClicked, this, [=]() { Q_EMIT elaScreenNavigation(); });
    // homeCard1->setTitle("ElaScreen");
    // homeCard1->setSubTitle("5.0⭐ 实用程序与工具");
    // homeCard1->setCardPixmap(QPixmap(":/Resource/Image/control/AutomationProperties.png"));
    // homeCard1->setInteractiveTips("免费使用");
    // homeCard1->setDetailedText("使用ElaDxgiManager获取屏幕的实时数据，以QImage的形式处理数据，支持切换采集设备和输出设备。");
    // homeCard1->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    // ElaPopularCard* homeCard2 = new ElaPopularCard(this);
    // connect(homeCard2, &ElaPopularCard::popularCardButtonClicked, this, [=]() { Q_EMIT elaSceneNavigation(); });
    // homeCard2->setTitle("ElaScene");
    // homeCard2->setSubTitle("5.0⭐ 实用程序与工具");
    // homeCard2->setCardPixmap(QPixmap(":/Resource/Image/control/Canvas.png"));
    // homeCard2->setInteractiveTips("免费使用");
    // homeCard2->setDetailedText("使用ElaScene封装的高集成度API进行快速拓扑绘图开发，对基于连接的网络拓扑特化处理。");
    // homeCard2->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    // ElaPopularCard* homeCard3 = new ElaPopularCard(this);
    // connect(homeCard3, &ElaPopularCard::popularCardButtonClicked, this, [=]() { Q_EMIT elaBaseComponentNavigation(); });
    // homeCard3->setTitle("ElaBaseComponent");
    // homeCard3->setSubTitle("5.0⭐ 实用程序与工具");
    // homeCard3->setCardPixmap(QPixmap(":/Resource/Image/control/StandardUICommand.png"));
    // homeCard3->setInteractiveTips("免费使用");
    // homeCard3->setDetailedText("添加ElaBaseComponent页面中的基础组件到你的项目中以进行快捷开发，使用方便，结构整洁，API规范");
    // homeCard3->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    // ElaPopularCard* homeCard4 = new ElaPopularCard(this);
    // connect(homeCard4, &ElaPopularCard::popularCardButtonClicked, this, [=]() { Q_EMIT elaCardNavigation(); });
    // homeCard4->setTitle("ElaCard");
    // homeCard4->setSubTitle("5.0⭐ 实用程序与工具");
    // homeCard4->setCardPixmap(QPixmap(":/Resource/Image/control/FlipView.png"));
    // homeCard4->setInteractiveTips("免费使用");
    // homeCard4->setDetailedText("使用ElaCard系列组件，包括促销卡片和促销卡片视窗来快速建立循环动画。");
    // homeCard4->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    // ElaPopularCard* homeCard5 = new ElaPopularCard(this);
    // connect(homeCard5, &ElaPopularCard::popularCardButtonClicked, this, [=]() { Q_EMIT elaIconNavigation(); });
    // homeCard5->setTitle("ElaIcon");
    // homeCard5->setSubTitle("5.0⭐ 实用程序与工具");
    // homeCard5->setCardPixmap(QPixmap(":/Resource/Image/control/CommandBarFlyout.png"));
    // homeCard5->setInteractiveTips("免费使用");
    // homeCard5->setDetailedText("在该界面快速挑选你喜欢的图标应用到项目中，以枚举的形式使用它");
    // homeCard5->setCardFloatPixmap(QPixmap(":/Resource/Image/IARC/IARC_7+.svg.png"));

    // ElaFlowLayout* flowLayout = new ElaFlowLayout(0, 5, 5);
    // flowLayout->setContentsMargins(30, 0, 0, 0);
    // flowLayout->setIsAnimation(true);
    // flowLayout->addWidget(homeCard);
    // flowLayout->addWidget(homeCard1);
    // flowLayout->addWidget(homeCard2);
    // flowLayout->addWidget(homeCard3);
    // flowLayout->addWidget(homeCard4);
    // flowLayout->addWidget(homeCard5);

    // // 菜单
    // _homeMenu = new ElaMenu(this);
    // ElaMenu* checkMenu = _homeMenu->addMenu(ElaIconType::Cubes, "查看");
    // checkMenu->addAction("查看1");
    // checkMenu->addAction("查看2");
    // checkMenu->addAction("查看3");
    // checkMenu->addAction("查看4");

    // ElaMenu* checkMenu1 = _homeMenu->addMenu(ElaIconType::Cubes, "查看");
    // checkMenu1->addAction("查看1");
    // checkMenu1->addAction("查看2");
    // checkMenu1->addAction("查看3");
    // checkMenu1->addAction("查看4");

    // ElaMenu* checkMenu2 = checkMenu->addMenu(ElaIconType::Cubes, "查看");
    // checkMenu2->addAction("查看1");
    // checkMenu2->addAction("查看2");
    // checkMenu2->addAction("查看3");
    // checkMenu2->addAction("查看4");

    // QKeySequence key = QKeySequence(Qt::CTRL | Qt::Key_S);

    // _homeMenu->addSeparator();
    // _homeMenu->addElaIconAction(ElaIconType::BoxCheck, "排序方式", QKeySequence::Save);
    // _homeMenu->addElaIconAction(ElaIconType::ArrowRotateRight, "刷新");
    // QAction* action = _homeMenu->addElaIconAction(ElaIconType::ArrowRotateLeft, "撤销");
    // connect(action, &QAction::triggered, this, [=]() {
    //     ElaNavigationRouter::getInstance()->navigationRouteBack();
    // });

    // _homeMenu->addElaIconAction(ElaIconType::Copy, "复制");
    // _homeMenu->addElaIconAction(ElaIconType::MagnifyingGlassPlus, "显示设置");

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Home");
    QVBoxLayout *centerVLayout = new QVBoxLayout(centralWidget);
    centerVLayout->setSpacing(0);
    centerVLayout->setContentsMargins(0, 0, 0, 0);
    centerVLayout->addWidget(backgroundCard);
    centerVLayout->addSpacing(20);
    // centerVLayout->addLayout(flowTextLayout);
    // centerVLayout->addSpacing(10);
    // centerVLayout->addLayout(flowLayout);
    // centerVLayout->addStretch();
    addCentralWidget(centralWidget);

    // 初始化提示
    ElaMessageBar::success(ElaMessageBarType::BottomRight, "Success", "初始化成功!", 2000);
    qDebug() << "初始化成功";
}

T_Home::~T_Home()
{
}

void T_Home::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::RightButton:
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        _homeMenu->popup(event->globalPosition().toPoint());
#else
        _homeMenu->popup(event->globalPos());
#endif
        break;
    }
    default:
    {
        break;
    }
    }
    ElaScrollPage::mouseReleaseEvent(event);
}



void T_Home::send_requests()
{
    if(!AppConfig::instance().isEnableTTS()){
        return;
    }

    // QString currentString = plainTextEdit2->toPlainText();
    if (received_txt==plainTextEdit2->toPlainText()){
        qDebug() << "未变化 播放旧音频";
        playAudio(response_data);
        return;
    }

    qDebug() << "clicked!";
    ServeTTSRequest request;

    request.text = plainTextEdit2->toPlainText();
    // "合成所需的音频并流式返回";

    // request.references.append(ServeReferenceAudio("audio1", "text1"));
    // request.references.append(ServeReferenceAudio("audio2", "text2"));
    request.reference_id = combx_voice->currentText();
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

    received_txt = plainTextEdit2->toPlainText();

    reply->deleteLater();
}

void T_Home::send_requests_after_ollama_auto()
{
    ServeTTSRequest request;

    request.text = plainTextEdit2->toPlainText();
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
    playAudio(response_data);
    received_txt = plainTextEdit2->toPlainText();
    reply->deleteLater();
}

void T_Home::send_requests_to_ollama()
{

    received_txt = "";
    qDebug() << "clicked!";
    OllamaRequest request;
    request.model = "qwen2-rp";
    request.stream = true;

    request.options.num_predict = promot_size_Edit1->text().toInt();

    Ollama_messages message_a{"assistant", textEdit1->text()};
    // message_a.role = "assistant";
    // message_a.content = "亲爱的，我是你的性感女友，我会为了你做任何事情。";
    Ollama_messages message_u{"user", plainTextEdit1->toPlainText()};
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

    parseResponse(reply);

    loop.exec();

    if(!AppConfig::instance().isEnableTTS()){
        return;
    }
    qDebug() << "收到回复 自动转tts。。。";
    send_requests_after_ollama_auto();
}


void T_Home::parseResponse(QNetworkReply *reply)
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
                        plainTextEdit2->clear();
                        plainTextEdit2->setPlainText(receiving_txt);

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

void T_Home::playAudio(const QByteArray &audioData)
{

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


    if (tempFile->open())
    {
        tempFile->write(audioData);
        tempFile->flush();
        tempFile->close();
    }

    QBuffer *audioBuffer = new QBuffer(this);
    audioBuffer->setData(audioData);
    audioBuffer->open(QIODevice::ReadOnly);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);

    format.setSampleFormat(QAudioFormat::SampleFormat::Int16);

    QAudioSink *audio = new QAudioSink(format, this);


    qDebug() << "start";
    audio->start(audioBuffer);
}
