#ifndef SENCE_VOICE_WEBSERVER_API
#define SENCE_VOICE_WEBSERVER_API

#include<QWebSocket>
#include<QWebSocketServer>

#include<QObject>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>


class senceVoiceWebServerApi: public QObject
{
Q_OBJECT

public:
    // static senceVoiceWebServerApi &instance()
    // {
    //     static senceVoiceWebServerApi instance;
    //     return instance;
    // }
    explicit senceVoiceWebServerApi(QObject *parent=nullptr);
    ~senceVoiceWebServerApi();

    void setServerUrl(QString url);
    void connectToServer();

    bool sendVoiceMsg(const QByteArray &msg);

    bool isConnect();

private:
    QString _server_url = "ws://127.0.0.1:27000/ws/transcribe?lang=auto";
    QWebSocket *_websocket_client;

    bool _isConnect = false;

Q_SIGNALS:
    void RecvTextMsg(QString text);

private:
    Q_SLOT void slotConnected();
    Q_SLOT void slotDisConnected();
    Q_SLOT void slotMessageReceived(const QString &message);
};


#endif
