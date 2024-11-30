#include "ollama_api.h"

#include "app_log.h"

namespace Ollama
{


bool OllamaAPI::send_message_to_server(QString &text)
{
    APP_LOG_DEBUG("send_message_to_ollama_server");
    QJsonObject requestJson = ollama_request.toJson();
    QJsonDocument requestJsonDoc(requestJson);

    QUrl url(ollam_url);
    QNetworkRequest netWorkRequest(url);
    netWorkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    netWorkRequest.setRawHeader("authorization", "Bearer YOUR_API_KEY");

    QEventLoop loop; //使用 QEventLoop 阻塞 等待请求完成
    QNetworkReply *reply = manager->post(netWorkRequest, requestJsonDoc.toJson());

    APP_LOG_DEBUG("posted!");

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    if (!parseResponse(reply))
    {
        APP_LOG_ERROR("parseResponse error");
        return false;
    }

    loop.exec();
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid())
    {
        int status = statusCode.toInt();
        APP_LOG_DEBUG("Status Code:" << status);
        if(status!=200){
            return false;
        }
    }
    text = _receiving_txt;
    ollama_request.messages.append(Ollama_messages("assistant", _receiving_txt));
    return true;
}


bool OllamaAPI::parseResponse(QNetworkReply *reply)
{
    _receiving_txt= "";
    APP_LOG_DEBUG("start parseResponse");
    _success = true;
    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
        static QByteArray buffer;
        buffer.append(reply->readAll());
        while (true)
        {
            // APP_LOG_DEBUG("in parseResponse");
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
                        // APP_LOG_DEBUG(_receiving_txt);
                        _receiving_txt += content;
                    }
                }
            }
            else
            {
                APP_LOG_DEBUG("Failed to parse JSON object:" << jsonObjectData);
                _success = false;
            }
        }

        if (reply->atEnd())
        {
            // APP_LOG_DEBUG("->>>>receivedone<<<<--");
        }
    });
    return _success;
}

void OllamaAPI::set_ollama_url(QString url)
{
    ollam_url = url;
}






}; // namespace Ollama
