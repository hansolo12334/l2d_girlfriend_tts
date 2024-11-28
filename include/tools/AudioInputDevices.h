#ifndef AUDIOINPUTDEVEICES_H
#define AUDIOINPUTDEVEICES_H
#include <QAudioSource>
#include <QMediaDevices>

#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

#include <QPixmap>

#include <QByteArray>
#include <QScopedPointer>

class AudioInputDevices : public QIODevice
{
    Q_OBJECT

public:
    AudioInputDevices(const QAudioFormat &format);

    void start();
    void stop();

    qreal level() const { return m_level; }

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

    qreal calculateLevel(const char *data, qint64 len) const;

signals:
    void levelChanged(qreal level);
    void signalInputAudioBytearrayData(const char *data, qint64 len);
private:
    const QAudioFormat m_format;
    qreal m_level = 0.0; // 0.0 <= m_level <= 1.0
};

#endif
