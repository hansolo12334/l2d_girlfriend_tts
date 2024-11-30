#ifndef MEDIA_MANAGER_H
#define MEDIA_MANAGER_H

#include <QAudioSink>
#include <QObject>

#include <QAudioFormat>
#include <QBuffer>
#include <QTimer>
#include <QtMath>

#include <QMediaDevices>
#include <QThread>

#include "AudioInputDevices.h"
#include "AudioOutputDevices.h"

class AudioHandler : public QObject
{
    Q_OBJECT

public:
    // static AudioHandler &instance()
    // {
    //     static AudioHandler instance;
    //     return instance;
    // }
    explicit AudioHandler(QObject *parent=nullptr);

    void playAudio(const QByteArray &audioData);
    void playAudio_pull(const QByteArray audioData);

    double get_audio_rms(){
        return _lastRms;
    }

    bool isPlayFinish() {
        return isOutputFinish;
    }

    QByteArray getInputAudioData(){
        return recordedAudioData;
    }


private:
    bool _isPlayingAudio = false;
    QAudioSink *_audio = nullptr;
    QBuffer *audioBuffer = nullptr;


    QTimer *output_timer = nullptr;
    QTimer *input_timer = nullptr;

    int input_time = 0;


    QScopedPointer<AudioInputDevices> inputDevice;
    QScopedPointer<AudioOutputDevices> outputDevice;
    QScopedPointer<QAudioSource> audioInputsource;
    QScopedPointer<QAudioSink> audioOutputsource;

    QAudioFormat inputAudioFormat;
    QAudioFormat outputAudioFormat;

    QMediaDevices *m_outputMediaDevices = nullptr;
    QIODevice *m_output;

    QByteArray m_audioByteArrayData;
    QByteArray m_audioOutputByteArryaData;
    QByteArray m_audioInputByteArryaData;
    QByteArray recordedAudioData;

    bool isInit = false;

    double normalizedVolume = 0;
    bool isOutputFinish = true;

    float _lastRms=0.0f;                  // 上一帧的 RMS 值
    QVector<float> normalizedData;
    int start = 0;
    char max_value = -1000;


private:
    void initializeInputAudio(const QAudioDevice &deviceInfo);
    void initializeOutPutAudio(const QAudioDevice &deviceInfo);

    void startAudioOutput();
    void stopAudioOutput();

    void startAudioInput();
    void stopAudioInput();

    //降采样输入数据
    QByteArray downsampleBuffer(const QByteArray &buffer, int inputSampleRate, int outputSampleRate);
    QByteArray encodePCM(const QByteArray &floatData);

    void predProcessData(QByteArray audioData);
    void predProcessData1(QByteArray audioData);
    void calculateRms(const QByteArray &audioData);
    void calculateRms(int len);
    template <typename T>
    T clamp(const T &value, const T &low, const T &high);

private:
    Q_SLOT void handleStateChanged(QAudio::State newState);
    Q_SLOT void onOutputNotify();
    Q_SLOT void onInputNotify();

public:
    // Q_SLOT void onStartTalking();
    // Q_SLOT void onStopTalking();
    void onStartTalking();
    void onStopTalking();

    Q_SLOT void playRecordedAudio();

Q_SIGNALS:
    void signalAudioLevel(qreal value);
    void playAudioRms(double value);
    void singalAudioSend(QByteArray data);

    // private:
    //     AudioHandler();
};


#endif
