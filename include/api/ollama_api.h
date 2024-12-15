#ifndef OLLAMA_API_H
#define OLLAMA_API_H

#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QSettings>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <QByteArray>
#include <QDebug>
#include <QUrl>

#include <optional>


namespace Ollama{


class Ollama_messages
{
public:
    QString role;
    QString content;


    Ollama_messages(const QString &role, const QString &content) : role{role}, content{content}
    {
    }
};

class OllamaRequest
{
public:
    QString model;
    bool stream = true;
    QList<Ollama_messages> messages;
    struct Options
    {
        int num_keep = 5;
        int num_predict = 60;
        int top_k = 40;
        float top_p = 0.95f;
        float temperature = 0.7f;
        float repeat_penalty = 1.0;
        bool penalize_newline = true;
        bool numa = false;
        int num_thread = 8;
    } options;

    QJsonObject toJson() const
    {
        QJsonObject json;
        json["model"] = model;
        json["stream"] = stream;

        QJsonArray messagesArray;
        for (const auto &message : messages)
        {
            QJsonObject messageJson;
            messageJson["role"] = message.role;
            messageJson["content"] = message.content;
            messagesArray.append(messageJson);
        }
        json["messages"] = messagesArray;

        QJsonObject optionsJson;
        optionsJson["num_keep"] = options.num_keep;
        optionsJson["num_predict"] = options.num_predict;
        optionsJson["top_k"] = options.top_k;
        optionsJson["top_p"] = options.top_p;
        optionsJson["temperature"] = options.temperature;
        optionsJson["repeat_penalty"] = options.repeat_penalty;
        optionsJson["penalize_newline"] = options.penalize_newline;
        optionsJson["numa"] = options.numa;
        optionsJson["num_thread"] = options.num_thread;
        json["options"] = optionsJson;

        return json;
    }
};






class OllamaAPI : public QObject
{
    Q_OBJECT
public:
    static OllamaAPI &instance()
    {
        static OllamaAPI instance;
        return instance;
    }

    Ollama::OllamaRequest ollama_request;

    bool send_message_to_server(QString &text);

    bool parseResponse(QNetworkReply *reply);

    void set_ollama_url(QString url);

    void set_model(QString model_name){
        ollama_request.model = model_name;
    }
    void set_stream(bool  value){
        ollama_request.stream = value;
    }

    void set_message_assistant(QString message){
        ollama_request.messages[0].content = message;
    }
    void set_token_size(int value){
        ollama_request.options.num_predict = value;
    }

    void add_message(Ollama_messages message){
        ollama_request.messages.append(message);
    }


private :
    bool isInit = false;
    QNetworkAccessManager *manager = nullptr;

    QString ollam_url = "http://127.0.0.1:6006/api/chat";
    QString  _receiving_txt= "";
    bool _success = true;
private:
    OllamaAPI()
    {
        if(!isInit)
        {
            isInit = true;
            manager = new QNetworkAccessManager(this);
            ollama_request.messages.append(Ollama_messages("assistant", ""));
        }
    }
    ~OllamaAPI(){}
};

}; // namespace Ollama
#endif
