#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QtWebSockets/QWebSocket>
#include <QBuffer>
#include <QAudio>     //这五个是QT处理音频的库
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QThread>
#include <QAudioSource>
#include <QAudioSink>
#include <QTimer>
#include <QFile>
#include <QMediaDevices>
#include "datahandle.h"
#include "audioInputdevices.h"
#include "audiooutputdevices.h"
class AudioHandler : public QObject
{
    Q_OBJECT
public:
    explicit AudioHandler(QObject *parent = nullptr);
    ~AudioHandler();
public slots:

    void onStartTalking();

    void onStopTalking();

    // void deviceChanged(QAudioDevice device, int index);

    void onAudioDataformWebsocket(const QByteArray& audioOutputData);
private slots:

    void processAudioData(const QByteArray &data);

    void onInputNotify();

    void onOutputNotify();

private:
    void initializeInputAudio(const QAudioDevice &deviceInfo);
    void initializeOutPutAudio(const QAudioDevice &deviceInfo);
    void startAudioInput();
    void stopAudioInput();
    void startAudioOutput();
    void stopAudioOutput();

private:
    QScopedPointer<AudioIputDevices> inputDevice;
    QScopedPointer<AudioOutputDevices> outputDevice;
    QIODevice *m_output;
    QAudioFormat inputAudioFormat;
    QAudioFormat outputAudioFormat;
    QScopedPointer<QAudioSource> audioInputsource;
    QScopedPointer<QAudioSink> audioOutputsource;
    // QAudioDevice inputDevice;
    // QAudioDevice outputAudioDevice;
    QMediaDevices *m_inputMediaDevices;
    QMediaDevices *m_outputMediaDevices;
    QTimer *m_inputTimer;
    QTimer *m_outputTimer;
    QTimer *m_pushTimer;
    QBuffer *m_audioBuffer;

    DataHandle m_dataHandle;
    QByteArray m_audioByteArrayData;
    QByteArray m_audioOutputByteArryaData;
signals:
    void signalSendData(const QByteArray& data);
    void signalRequestTalk(CMDTYPE cmdtype);
    void signalAudioLevel(qreal value);
};

#endif // AUDIOHANDLER_H
