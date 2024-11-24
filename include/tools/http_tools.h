#ifndef HTTP_TOOLS_H
#define HTTP_TOOLS_H

#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>

#include<QNetworkAccessManager>
#include<QNetworkRequest>
#include<QNetworkReply>

#include<QUrl>
#include<QByteArray>
#include<QDebug>

#include <optional>


class ServeReferenceAudio
{
public:
    QString audio;
    QString text;

    ServeReferenceAudio(const QString &audio,const QString &text)
     : audio{audio}
     , text{text}
     {}
};

class ServeTTSRequest
{
public:
    QString text;
    int chunk_length = 200;
    QString format = "wav";
    int mp3_bitrate=128;
    QList<ServeReferenceAudio> references;
    QString reference_id;
    std::optional<int> seed;
    QString use_memory_cache = "never";
    bool normalize = true;
    int opus_bitrate = -1000;
    QString latency = "normal";
    bool streaming = false;
    int max_new_tokens = 1024;
    float top_p = 0.7f;
    float repetition_penalty = 1.2f;
    float temperature = 0.7f;

    QJsonObject toJson() const{
        QJsonObject json;
        json["text"] = text;
        json["chunk_length"] = chunk_length;
        json["format"] = format;
        json["mp3_bitrate"] = mp3_bitrate;

        QJsonArray referencesArray;
        for (const auto& ref : references) {
            QJsonObject refJson;
            refJson["audio"] = ref.audio;
            refJson["text"] = ref.text;
            referencesArray.append(refJson);
        }
        json["references"] = referencesArray;
        json["reference_id"] = reference_id;
        if(seed.has_value()){
            json["seed"] = seed.value();
        }else{
            json["seed"] = QJsonValue::Null;
        }

        json["use_memory_cache"] = use_memory_cache;
        json["normalize"] = normalize;
        json["opus_bitrate"] = opus_bitrate;
        json["latency"] = latency;
        json["streaming"] = streaming;
        json["max_new_tokens"] = max_new_tokens;
        json["top_p"] = top_p;
        json["repetition_penalty"] = repetition_penalty;
        json["temperature"] = temperature;
        return json;
    }
};



class Ollama_messages
{
public:
    QString role;
    QString content;


    Ollama_messages(const QString &role, const QString &content) : role{role}, content{content}
    {}
};

class OllamaRequest
{
public:
    QString model;
    bool stream = true;
    QList<Ollama_messages> messages;
    struct Options {
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

    QJsonObject toJson() const {
        QJsonObject json;
        json["model"] = model;
        json["stream"] = stream;

        QJsonArray messagesArray;
        for (const auto& message : messages) {
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


#endif
