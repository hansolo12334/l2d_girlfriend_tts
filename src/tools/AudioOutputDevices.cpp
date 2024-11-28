#include <QAudioDevice>
#include <QAudioSink>
#include <QDebug>
#include <QVBoxLayout>
#include <QtEndian>
#include <QtMath>

#include "AudioOutputDevices.h"

AudioOutputDevices::AudioOutputDevices(const QAudioFormat &format, qint64 durationUs, int sampleRate)
{
    if (format.isValid())
        generateData(format, durationUs, sampleRate);
}

void AudioOutputDevices::start()
{
    open(QIODevice::ReadOnly);
}

void AudioOutputDevices::stop()
{
    m_pos = 0;
    close();
}

void AudioOutputDevices::generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate)
{
    const int channelBytes = format.bytesPerSample();
    const int sampleBytes = format.channelCount() * channelBytes;
    qint64 length = format.bytesForDuration(durationUs);
    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED(sampleBytes); // suppress warning in release builds

    m_buffer.resize(length);
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
    int sampleIndex = 0;

    while (length) {
        // Produces value (-1..1)
        const qreal x = qSin(2 * M_PI * sampleRate * qreal(sampleIndex++ % format.sampleRate())
                             / format.sampleRate());
        for (int i = 0; i < format.channelCount(); ++i) {
            switch (format.sampleFormat()) {
            case QAudioFormat::UInt8:
                *reinterpret_cast<quint8 *>(ptr) = static_cast<quint8>((1.0 + x) / 2 * 255);
                break;
            case QAudioFormat::Int16:
                *reinterpret_cast<qint16 *>(ptr) = static_cast<qint16>(x * 32767);
                break;
            case QAudioFormat::Int32:
                *reinterpret_cast<qint32 *>(ptr) =
                    static_cast<qint32>(x * std::numeric_limits<qint32>::max());
                break;
            case QAudioFormat::Float:
                *reinterpret_cast<float *>(ptr) = x;
                break;
            default:
                break;
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
    }
}

qint64 AudioOutputDevices::readData(char *data, qint64 len)
{
    qint64 total = 0;
//    if (!m_buffer.isEmpty()) {
//        // qDebug() << "!m_buffer.isEmpty()" << m_buffer ;
//        while (len - total > 0) {
//            const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
//            memcpy(data + total, m_buffer.constData() + m_pos, chunk);
//            m_pos = (m_pos + chunk) % m_buffer.size();
//            total += chunk;
//        }
//    }
    return total;
}

qint64 AudioOutputDevices::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 AudioOutputDevices::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}
