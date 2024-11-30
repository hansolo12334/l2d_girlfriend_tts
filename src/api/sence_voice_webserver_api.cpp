#include"sence_voice_webserver_api.h"

#include"app_log.h"


senceVoiceWebServerApi::senceVoiceWebServerApi(QObject *parent)
    : QObject(parent)
{
    _websocket_client = new QWebSocket();

    connect(_websocket_client, &QWebSocket::connected, this, &senceVoiceWebServerApi::slotConnected);
    connect(_websocket_client, &QWebSocket::disconnected, this, &senceVoiceWebServerApi::slotDisConnected);
    connect(_websocket_client, &QWebSocket::textMessageReceived, this, &senceVoiceWebServerApi::slotMessageReceived);
}

senceVoiceWebServerApi::~senceVoiceWebServerApi()
{

}

void senceVoiceWebServerApi::slotMessageReceived(const QString &message)
{
    // qDebug() << "receive message: " << message;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    if(jsonObj["code"]==0){
        QString re = jsonObj["data"].toString();
        qDebug() << "receive message: " << re;
        if(!re.isEmpty()){
            emit RecvTextMsg(re);
        }

    }
}


void senceVoiceWebServerApi::slotConnected()
{
    APP_LOG_DEBUG("webserver to server connect!");
    _isConnect = true;
}

void senceVoiceWebServerApi::slotDisConnected()
{
    APP_LOG_DEBUG("webserver to server disconnect! ");
    _isConnect = false;
}

void senceVoiceWebServerApi::setServerUrl(QString url)
{
    _server_url = url;
}

bool senceVoiceWebServerApi::isConnect()
{
    return _isConnect;
}


void senceVoiceWebServerApi::connectToServer()
{
    _websocket_client->open(QUrl(_server_url));
}

bool senceVoiceWebServerApi::sendVoiceMsg(const QByteArray &msg)
{
    if (_websocket_client->isValid()) {
        auto size=_websocket_client->sendBinaryMessage(msg);
        // qDebug() << "Sending audio data:" << size << "bytes";
    } else {
        qDebug() << "WebSocket is not connected";
        return false;
    }
    return true;
}
