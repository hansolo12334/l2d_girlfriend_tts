#include "media_manager.h"


#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioSink>
#include <QBuffer>
#include <QEventLoop>
#include <QMediaPlayer>


#include "app_config.h"


AudioHandler::AudioHandler()
{
    if (!isInit)
    {
        isInit = true;
        for (int i = 0; i < QMediaDevices::audioInputs().count(); ++i)
        {
            auto aa = QMediaDevices::audioInputs().at(i);
            qDebug() << "音频输入：" << aa.description();
        }
        for (int i = 0; i < QMediaDevices::audioOutputs().count(); ++i)
        {
            auto aa = QMediaDevices::audioOutputs().at(i);
            qDebug() << "音频输出：" << aa.description();
        }

        output_timer = new QTimer(this);
        input_timer = new QTimer(this);

        connect(input_timer, &QTimer::timeout, this, &AudioHandler::onInputNotify);
        connect(inputDevice.data(), &AudioInputDevices::levelChanged, [=](qreal value){
        //        qDebug() << "emit signalAudioLevel(value);" << value;
            emit signalAudioLevel(value);
        });
        connect(output_timer, &QTimer::timeout, this, &AudioHandler::onOutputNotify);

        onStopTalking();
    }
}


void AudioHandler::initializeInputAudio(const QAudioDevice &deviceInfo)
{
    //设置录音的格式
    inputAudioFormat.setSampleRate(48000); //44100 设置采样率以对赫兹采样。 以秒为单位，每秒采集多少声音数据的频率.
    inputAudioFormat.setChannelCount(1); //将通道数设置为通道。
    // audioFormat.setSampleSize(16);     /*将样本大小设置为指定的sampleSize（以位为单位）通常为8或16，但是某些系统可能支持更大的样本量。*/
    inputAudioFormat.setSampleFormat(QAudioFormat::Int16);
    // audioFormat.setCodec("audio/pcm"); //设置编码格式
    // audioFormat.setByteOrder(QAudioFormat::LittleEndian); //样本是小端字节顺序
    // audioFormat.setSampleType(QAudioFormat::SignedInt); //样本类型

    // ChannelConfigStereo is 2, Int16 is 2
    qDebug("sampleRate: %d, channelCount: %d, sampleFormat: %d",
           inputAudioFormat.sampleRate(),
           inputAudioFormat.channelCount(),
           inputAudioFormat.sampleFormat());

    inputDevice.reset(new AudioInputDevices(inputAudioFormat));

    audioInputsource.reset(new QAudioSource(deviceInfo, inputAudioFormat));

    connect(inputDevice.data(), &AudioInputDevices::signalInputAudioBytearrayData, [=](const char *data, qint64 len) {
        // qDebug() << "m_audioByteArrayData start:" << "m_audioByteArrayData size:" << m_audioByteArrayData.size() ; hansolo
        QByteArray aa(data, len);
        m_audioByteArrayData.append(aa);
        // qDebug() << "m_audioByteArrayData final:" << "m_audioByteArrayData size:" << m_audioByteArrayData.size() ; hansolo
    });
}


void AudioHandler::initializeOutPutAudio(const QAudioDevice &deviceInfo)
{
    qDebug() << "outputAudioDevice音频输出：" << deviceInfo.description();

    outputAudioFormat = deviceInfo.preferredFormat();

    outputAudioFormat.setSampleRate(48000); //8000设置采样率以对赫兹采样。 以秒为单位，每秒采集多少声音数据的频率.
    outputAudioFormat.setChannelCount(1); //将通道数设置为通道。
    // audioFormat.setSampleSize(16);     /*将样本大小设置为指定的sampleSize（以位为单位）通常为8或16，但是某些系统可能支持更大的样本量。*/
    outputAudioFormat.setSampleFormat(QAudioFormat::Int16);

    if (!deviceInfo.isFormatSupported(outputAudioFormat))
    {
        qWarning() << "not Support fromat";
    }

    qDebug("sampleRate: %d, channelCount: %d, sampleFormat: %d",
           outputAudioFormat.sampleRate(),
           outputAudioFormat.channelCount(),
           outputAudioFormat.sampleFormat());

    const int durationSeconds = 1;
    const int toneSampleRateHz = 600;
    outputDevice.reset(new AudioOutputDevices(outputAudioFormat, durationSeconds * 1000000, toneSampleRateHz));
    audioOutputsource.reset(new QAudioSink(deviceInfo, outputAudioFormat));
}






void AudioHandler::playAudio(const QByteArray &audioData)
{
    if (_isPlayingAudio && _audio)
    {
        APP_LOG_DEBUG("正在播放 停止播放");
        _audio->stop();
        delete _audio;
        _audio = nullptr;
        _isPlayingAudio = false;
    }

    audioBuffer = new QBuffer(this);
    audioBuffer->setData(audioData);
    audioBuffer->open(QIODevice::ReadOnly);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::SampleFormat::Int16);

    _audio = new QAudioSink(format, this);


    APP_LOG_DEBUG("start play");
    _audio->start(audioBuffer);
    _isPlayingAudio = true;


    connect(_audio, &QAudioSink::stateChanged, this, [=](QAudio::State state) {
        if (state == QAudio::IdleState)
        {
            _isPlayingAudio = false;
            audioBuffer->deleteLater();
            _audio->deleteLater();
            _audio = nullptr;
            APP_LOG_DEBUG("stop play");
            // timer->stop();
            // timer->deleteLater();
            // timer = nullptr;
        }
    });

}


void AudioHandler::handleStateChanged(QAudio::State newState)
{
    switch (newState)
    {
    case QAudio::IdleState:
        // Finished playing (no more data)
        _audio->stop();
        audioBuffer->deleteLater();
        _audio->deleteLater();
        _audio = nullptr;
        APP_LOG_DEBUG("stop play");
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        if (_audio->error() != QAudio::NoError)
        {
            APP_LOG_ERROR("Stopped for other reasons");
        }
        break;

    default:
        // ... other cases as appropriate
        break;
    }
}

void AudioHandler::calculateRms(const QByteArray &audioData)
{
    const int16_t *data = reinterpret_cast<const int16_t *>(audioData.constData());
    int sampleCount = audioData.size() / sizeof(int16_t);

    double sum = 0;
    for (int i = 0; i < sampleCount; ++i)
    {
        sum += qPow(data[i], 2);
    }

    double rms = qSqrt(sum / sampleCount);
    APP_LOG_DEBUG("RMS:" << rms);
}



void AudioHandler::onOutputNotify()
{
    if(m_audioOutputByteArryaData.size() == 0){
        qDebug() << "m_audioOutputByteArryaData<0 ";
            return;
    }
    qDebug() << "m_audioOutputByteArryaData大小:" << m_audioOutputByteArryaData.size();

           // auto io = audioOutputsource->start();
           // int len = audioOutputsource->bytesFree();
           // qDebug() << "len:" << len;
           // len = io->write(m_audioOutputByteArryaData.data(), m_audioOutputByteArryaData.size());

    int len = audioOutputsource->bytesFree();
    qDebug() << "len:" << len;
    calculateRms(m_audioOutputByteArryaData);
    // len = m_output->write(m_audioOutputByteArryaData.data(), m_audioOutputByteArryaData.size());
    // m_audioOutputByteArryaData.clear();
    //test
    if (len > 0)
    {
        QByteArray dataToWrite = m_audioOutputByteArryaData.left(len);
        int written = m_output->write(dataToWrite.data(), dataToWrite.size());
        m_audioOutputByteArryaData.remove(0, written);
        qDebug()<<"剩余大小:: "<<m_audioOutputByteArryaData.size();
    }

    if (m_audioOutputByteArryaData.isEmpty())
    {
        m_outputTimer->stop();
        stopAudioOutput();
    }

}


void AudioHandler::onInputNotify()
{
    // Read available audio input data and send it
    if (inputDevice)
    {
        //hansolo
        m_audioOutputByteArryaData.append(m_audioByteArrayData);
        m_audioByteArrayData.clear();
    }

}


void AudioHandler::onStartTalking()
{
    qDebug() << "onStartTalking";
    stopAudioOutput(); //hansolo
    startAudioInput();
    // emit signalRequestTalk(CMDTYPE::CMDTALK);
}

void AudioHandler::onStopTalking()
{
    qDebug() << "onStopTalking";
    // stopAudioInput(); hansolo
    // startAudioOutput();  hansolo


    //test
    stopAudioInput();
    qDebug()<<"m_audioByteArrayData.isEmpty() "<<m_audioByteArrayData.isEmpty();
    recordedAudioData = m_audioOutputByteArryaData;
    m_audioByteArrayData.clear();

}


void AudioHandler::playRecordedAudio()
{
    qDebug() << "playRecordedAudio";
    qDebug()<<recordedAudioData.isEmpty();
    if (!recordedAudioData.isEmpty())
    {
        // m_audioOutputByteArryaData = recordedAudioData;
        startAudioOutput();
    }
}

void AudioHandler::startAudioInput()
{
    if (audioInputsource)
    {
        inputDevice->start();
        audioInputsource->start(inputDevice.data());
        m_inputTimer->start(10);
        // connect(inputDevice, &QIODevice::readyRead, this, &AudioHandler::onNotify);
    }
}

void AudioHandler::stopAudioInput()
{
    if (audioInputsource)
    {
        m_inputTimer->stop();
        audioInputsource->stop();
    }
}

void AudioHandler::startAudioOutput()
{
    qDebug() << "onstartListening";
    outputDevice->start();
    m_output = audioOutputsource->start();
    m_outputTimer->start(10);
}

void AudioHandler::stopAudioOutput()
{
    qDebug() << "onstopListening";
    m_outputTimer->stop();
    outputDevice->stop();
    audioOutputsource->stop();
}
