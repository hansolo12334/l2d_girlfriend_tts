#ifndef TTS_API_H
#define TTS_API_H

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



namespace TTS
{

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




class TTSAPI :public QObject
{
Q_OBJECT
public:
    static TTSAPI &instance()
    {
        static TTSAPI instance;
        return instance;
    }

    bool send_message_to_server(TTS::ServeTTSRequest request,QByteArray &response_data);

    void set_tts_url(QString url);

private:
    bool isInit = false;
    QNetworkAccessManager *manager = nullptr;

    QString tts_url = "http://localhost:11434/api/chat"; ///
    bool _success = true;

private:
    TTSAPI()
    {
        if(!isInit)
        {
            isInit = true;
            manager = new QNetworkAccessManager(this);
        }
    }
    ~TTSAPI(){}
};






}; // namespace TTS


#endif
