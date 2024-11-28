#ifndef AUDIOOUTPUTDEVICES
#define AUDIOOUTPUTDEVICES

#include <QAudioSink>
#include <QByteArray>
#include <QComboBox>
#include <QIODevice>
#include <QLabel>
#include <QMainWindow>
#include <QMediaDevices>
#include <QObject>
#include <QPushButton>
#include <QScopedPointer>
#include <QSlider>
#include <QTimer>

class AudioOutputDevices : public QIODevice
{
    Q_OBJECT

public:
    AudioOutputDevices(const QAudioFormat &format, qint64 durationUs, int sampleRate);

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    qint64 size() const override { return m_buffer.size(); }

private:
    void generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate);

private:
    qint64 m_pos = 0;
    QByteArray m_buffer;
};


#endif
