#include <QAudioDevice>
#include <QAudioSource>
#include <QDateTime>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QtEndian>

#include"AudioInputDevices.h"

#if QT_CONFIG(permissions)
#include <QCoreApplication>
#include <QPermission>
#endif

#include <math.h>
#include <stdlib.h>

AudioInputDevices::AudioInputDevices(const QAudioFormat &format) : m_format(format) { }

void AudioInputDevices::start()
{
    open(QIODevice::WriteOnly);
}

void AudioInputDevices::stop()
{
    close();
}

qint64 AudioInputDevices::readData(char * /* data */, qint64 /* maxlen */)
{
    return 0;
}

qreal AudioInputDevices::calculateLevel(const char *data, qint64 len) const
{
    const int channelBytes = m_format.bytesPerSample();
    const int sampleBytes = m_format.bytesPerFrame();
    const int numSamples = len / sampleBytes;

    float maxValue = 0;
    auto *ptr = reinterpret_cast<const unsigned char *>(data);

    for (int i = 0; i < numSamples; ++i) {
        for (int j = 0; j < m_format.channelCount(); ++j) {
            float value = m_format.normalizedSampleValue(ptr);

            maxValue = qMax(value, maxValue);
            ptr += channelBytes;
        }
    }
    return maxValue;
}

qint64 AudioInputDevices::writeData(const char *data, qint64 len)
{
    m_level = calculateLevel(data, len);
    emit signalInputAudioBytearrayData(data, len);
    emit levelChanged(m_level);
    return len;
}
