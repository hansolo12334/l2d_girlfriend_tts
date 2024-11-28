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
    static AudioHandler &instance()
    {
        static AudioHandler instance;
        return instance;
    }

    void playAudio(const QByteArray &audioData);

private:
    bool _isPlayingAudio = false;
    QAudioSink *_audio = nullptr;
    QBuffer *audioBuffer = nullptr;


    QTimer *output_timer = nullptr;
    QTimer *input_timer = nullptr;


    QScopedPointer<AudioInputDevices> inputDevice;
    QScopedPointer<AudioOutputDevices> outputDevice;
    QScopedPointer<QAudioSource> audioInputsource;
    QScopedPointer<QAudioSink> audioOutputsource;

    QTimer *m_inputTimer;
    QTimer *m_outputTimer;

    QAudioFormat inputAudioFormat;
    QAudioFormat outputAudioFormat;

    QMediaDevices *m_outputMediaDevices = nullptr;
    QIODevice *m_output;

    QByteArray m_audioByteArrayData;
    QByteArray m_audioOutputByteArryaData;
    QByteArray recordedAudioData;

    bool isInit = false;


private:
    void initializeInputAudio(const QAudioDevice &deviceInfo);
    void initializeOutPutAudio(const QAudioDevice &deviceInfo);

    void startAudioOutput();
    void stopAudioOutput();

    void startAudioInput();
    void stopAudioInput();


    void calculateRms(const QByteArray &audioData);

private:
    Q_SLOT void handleStateChanged(QAudio::State newState);
    Q_SLOT void onOutputNotify();
    Q_SLOT void onInputNotify();

public:
    Q_SLOT void onStartTalking();
    Q_SLOT void onStopTalking();
    Q_SLOT void playRecordedAudio();

Q_SIGNALS:
    void signalAudioLevel(qreal value);

private:
    AudioHandler();
};


#endif
