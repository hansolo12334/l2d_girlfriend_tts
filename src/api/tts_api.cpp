#include"tts_api.h"
#include"app_log.h"

namespace TTS
{

bool TTSAPI::send_message_to_server(TTS::ServeTTSRequest request,QByteArray &response_data)
{
    QJsonObject requestJson = request.toJson();
    QJsonDocument requestJsonDoc(requestJson);
    // QByteArray requestData = doc.toJson();

    // qDebug() << requestData;

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
    reply->deleteLater();
    return true;
}

void TTSAPI::set_tts_url(QString url)
{
    tts_url = url;
}


};
