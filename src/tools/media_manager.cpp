#include "media_manager.h"


#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioSink>
#include <QBuffer>
#include <QEventLoop>
#include <QMediaPlayer>


#include "app_config.h"
#include "LAppPal.hpp"

AudioHandler::AudioHandler(QObject *parent)
 : QObject(parent)
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

        initializeInputAudio(QMediaDevices::defaultAudioInput());
        m_outputMediaDevices = new QMediaDevices(this);
        initializeOutPutAudio(m_outputMediaDevices->defaultAudioOutput());

        output_timer = new QTimer(this);
        input_timer = new QTimer(this);

        connect(input_timer, &QTimer::timeout, this, &AudioHandler::onInputNotify);
        // connect(inputDevice.data(), &AudioInputDevices::levelChanged, [=](qreal value){
        // //        qDebug() << "emit signalAudioLevel(value);" << value;
        //     emit signalAudioLevel(value);
        // });
        connect(output_timer, &QTimer::timeout, this, &AudioHandler::onOutputNotify);

        onStopTalking();
    }
}


void AudioHandler::initializeInputAudio(const QAudioDevice &deviceInfo)
{
    //设置录音的格式
    inputAudioFormat.setSampleRate(16000); //44100 设置采样率以对赫兹采样。 以秒为单位，每秒采集多少声音数据的频率.
    inputAudioFormat.setChannelCount(1); //将通道数设置为通道。
    // audioFormat.setSampleSize(16);     /*将样本大小设置为指定的sampleSize（以位为单位）通常为8或16，但是某些系统可能支持更大的样本量。*/
    inputAudioFormat.setSampleFormat(QAudioFormat::Int16);

    // audioFormat.setCodec("audio/pcm"); //设置编码格式
    // audioFormat.setByteOrder(QAudioFormat::LittleEndian); //样本是小端字节顺序
    // audioFormat. (QAudioFormat::SignedInt); //样本类型

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
        // qDebug() << "m_audioByteArrayData final:" << "m_audioByteArrayData size:" << m_audioByteArrayData.size() ; //hansolo
    });
}


void AudioHandler::initializeOutPutAudio(const QAudioDevice &deviceInfo)
{
    qDebug() << "outputAudioDevice音频输出：" << deviceInfo.description();

    // outputAudioFormat = deviceInfo.preferredFormat();

    outputAudioFormat.setSampleRate(44100); //8000设置采样率以对赫兹采样。 以秒为单位，每秒采集多少声音数据的频率.
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


void AudioHandler::playAudio_pull(const QByteArray audioData)
{
    predProcessData1(audioData);
    APP_LOG_DEBUG("audioData.size " << audioData.size());
    m_audioOutputByteArryaData = audioData;
    if (!audioData.isEmpty())
    {
        startAudioOutput();
    }
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



QByteArray AudioHandler::downsampleBuffer(const QByteArray &buffer, int inputSampleRate, int outputSampleRate)
{
    if (outputSampleRate == inputSampleRate) {
        return buffer;
    }

    int sampleRateRatio = inputSampleRate / outputSampleRate;
    int newLength = buffer.size() / sampleRateRatio;
    QByteArray result;
    result.resize(newLength);

    const int16_t *input = reinterpret_cast<const int16_t*>(buffer.constData());
    int16_t *output = reinterpret_cast<int16_t*>(result.data());

    for (int i = 0, j = 0; i < newLength; ++i, j += sampleRateRatio) {
        output[i] = input[j];
    }

    return result;
}

QByteArray AudioHandler::encodePCM(const QByteArray &floatData)
{
    int sampleBits = 16;
    int dataLength = floatData.size() * (sampleBits / 8);
    QByteArray buffer;
    buffer.resize(dataLength);

    const float *input = reinterpret_cast<const float*>(floatData.constData());
    int16_t *output = reinterpret_cast<int16_t*>(buffer.data());

    for (int i = 0; i < floatData.size() / sizeof(float); ++i) {
        float sample = std::max(-1.0f, std::min(1.0f, input[i]));
        output[i] = static_cast<int16_t>(sample < 0 ? sample * 0x8000 : sample * 0x7FFF);
    }

    return buffer;
}

void AudioHandler::predProcessData(QByteArray audioData)
{
    // int sampleRate = *reinterpret_cast<const int*>(&audioData[24]);  // 采样率（通常在字节24-27位置）
    // int numChannels = *reinterpret_cast<const short*>(&audioData[22]); // 通道数（通常在字节22-23位置）
    // int bitsPerSample = *reinterpret_cast<const short*>(&audioData[34]); //
    // qDebug() << "Sample Rate:" << sampleRate;
    // qDebug() << "Channels:" << numChannels;
    // qDebug() << "Bits per Sample:" << bitsPerSample;

    // // 读取音频数据部分：跳过44字节的WAV头部
    // const int16_t *data = reinterpret_cast<const int16_t *>(audioData.constData() + 44);
    // int sampleCount = (audioData.size() - 44) / sizeof(int16_t);

    // // 归一化音频数据，将16位音频数据从[-32768, 32767]映射到[-1.0, 1.0]
    // // QFile file("rms_values.txt");
    // // file.open(QIODevice::Append | QIODevice::Text);
    // // QTextStream out(&file);

    // normalizedData.clear();
    // for (int i = 0; i < sampleCount; ++i)
    // {
    //     // normalizedData[i] = static_cast<float>(data[i]+16384) / 32768.0f;
    //     normalizedData.push_back(static_cast<float>(data[i] + 16384) / 32768.0f);
    //     // out << normalizedData[i] << "\n";
    // }


    // const int16_t *data = reinterpret_cast<const int16_t *>(audioData.constData());
    // int sampleCount = audioData.size() / sizeof(int16_t);  // 获取当前数据片段的样本数
    // qDebug()<<"sampleCount " << sampleCount;

    // normalizedData.clear();


    // // 累加样本的平方值
    // for (int i = 0; i < sampleCount; ++i) {
    //     normalizedData.push_back(static_cast<float>(data[i] + 16384) / 32768.0f);
    // }

    for (int j = 0; j < audioData.size(); j++)
    {
        auto te = audioData[j];
        if(te>max_value){
            max_value = te;
        }
    }
    qDebug() << "maxvalue " << max_value;
}

void AudioHandler::predProcessData1(QByteArray audioData)
{
    start = 0;

    const int16_t *data = reinterpret_cast<const int16_t *>(audioData.constData());
    int sampleCount = (audioData.size()) / sizeof(int16_t);
    normalizedData.clear();

    // QFile file("rms_values.txt");
    // file.open(QIODevice::Append | QIODevice::Text);
    // QTextStream out(&file);
    for (int i = 0; i < sampleCount; ++i)
    {
        // normalizedData[i] = static_cast<float>(data[i]+16384) / 32768.0f;
        // normalizedData.push_back(static_cast<float>(data[i] + 16384) / 32768.0f);
        normalizedData.push_back(static_cast<float>(data[i]) / 32768.0f);
        // out << normalizedData[i] << "\n";
    }
    // file.close();
}


void AudioHandler::calculateRms(int len)
{
    float rms = 0.0f;

    int end = start + len/ 2;
    for (int i = start; i < end; ++i)
    {
        rms += normalizedData[i] * normalizedData[i];
    }

    rms = sqrt(rms / (len/2));

    rms = rms * 10;
    if(rms>1.0){
        rms=1.0;
    }else if(rms<0.1 || qIsNaN(rms)){
        rms = 0.1;
    }
    _lastRms = rms;
    start = end;

    QFile file("rms_values.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << _lastRms << "\n";
        file.close();
    } else {
        APP_LOG_ERROR("Unable to open file to save RMS values.");
    }
}


void AudioHandler::calculateRms(const QByteArray &audioData)
{

    const int16_t *data = reinterpret_cast<const int16_t *>(audioData.constData());
    int sampleCount = audioData.size() / sizeof(int16_t);  // 获取当前数据片段的样本数

    // 计算 RMS 值
    float rms = 0.0f;

    // 累加样本的平方值
    for (int i = 0; i < sampleCount; ++i) {
        rms += data[i] * data[i];
    }

    // 计算平均 RMS
    rms = sqrt(rms / sampleCount);

    // rms = log(rms + 1.0f) * 1000.0f;  // 对RMS进行对数缩放，增加动态响应
    // 设置最小 RMS 阈值
    if (rms < 0.01f) {
        rms = 0.01f;
    }
    rms = rms / 3600;

    // 更新 RMS 值
    _lastRms = rms;

    // 输出调试信息
    // qDebug() << "RMS:" << rms;

    // QFile file("rms_values.txt");
    // if (file.open(QIODevice::Append | QIODevice::Text)) {
    //     QTextStream out(&file);
    //     out << _lastRms << "\n";
    //     file.close();
    // } else {
    //     APP_LOG_ERROR("Unable to open file to save RMS values.");
    // }

    // 输出调试信息


    // const int16_t *data = reinterpret_cast<const int16_t *>(audioData.constData());
    // int sampleCount = audioData.size() / sizeof(int16_t);

    // double sum = 0;
    // for (int i = 0; i < sampleCount; ++i)
    // {
    //     sum += qPow(data[i], 2);
    // }

    // rms = qSqrt(sum / sampleCount);

    // if(rms<20){
    //     rms = 20;
    // }
    // QFile file("rms_values.txt");
    // if (file.open(QIODevice::Append | QIODevice::Text)) {
    //     QTextStream out(&file);
    //     out << rms << "\n";
    //     file.close();
    // } else {
    //     APP_LOG_ERROR("Unable to open file to save RMS values.");
    // }

    // APP_LOG_DEBUG("RMS:" << rms);
    // double dBValue = 20 * std::log10(rms);
    // constexpr double minDb = 24;
    // constexpr double maxDb = 70.0;
    // // Normalize decibel value to 0 - 1 range
    // normalizedVolume = (dBValue - minDb) / (maxDb - minDb);
    // normalizedVolume = clamp(normalizedVolume, 0.0, 1.0);

    // // APP_LOG_DEBUG("RMS:" << rms);
    // // APP_LOG_DEBUG("normalizedVolume:" << normalizedVolume);
}

template<typename T>
T AudioHandler::clamp(const T& value, const T& low, const T& high)
{
    return std::max(low, std::min(value, high));
}


void AudioHandler::onOutputNotify()
{

    int len = audioOutputsource->bytesFree();
    // qDebug() << "len:" << len;


    //test 将十六进制字符串转换为字节数组
    if (len > 0)
    {
        QByteArray dataToWrite = m_audioOutputByteArryaData.left(len);
        int written = m_output->write(dataToWrite.data(), dataToWrite.size());

        // calculateRms(len);
        calculateRms(dataToWrite.size());
        emit playAudioRms(_lastRms);

        m_audioOutputByteArryaData.remove(0, written);
       \
    }

    //audioOutputByteArryaData<0 读取完毕 ==>但是可能设备还没播放完音频 需要判断
    if(m_audioOutputByteArryaData.size()<=0)
    {
        switch (audioOutputsource->state())
        {
        case QAudio::State::IdleState:
            // qDebug() << "QAudio State==>IdleState";
            stopAudioOutput();
            break;
        case QAudio::State::ActiveState:
            // qDebug() << "QAudio State==>ActiveState";
            break;
        case QAudio::State::StoppedState:
            qDebug() << "QAudio State==>StoppedState";
            break;
        case QAudio::State::SuspendedState:
                qDebug() << "QAudio State==>SuspendedState";
                break;
        default:
            break;
        }
    }

}


void AudioHandler::onInputNotify()
{
    // Read available audio input data and send it
    if (inputDevice)
    {
        //hansolo
        m_audioInputByteArryaData.append(m_audioByteArrayData);
        m_audioByteArrayData.clear();
        input_time += 10;
        if(input_time>=500){
            input_time = 0;
            emit singalAudioSend(m_audioInputByteArryaData);
            m_audioInputByteArryaData.clear();
        }
    }
}


void AudioHandler::onStartTalking()
{

    qDebug() << "onStartTalking";
    stopAudioOutput(); //hansolo

    m_audioInputByteArryaData.clear();
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
    // qDebug()<<"m_audioByteArrayData.isEmpty() "<<m_audioByteArrayData.isEmpty();
    recordedAudioData = m_audioInputByteArryaData;
    qDebug()<<"recordedAudioData.isEmpty() "<<recordedAudioData.isEmpty();
    qDebug()<<"recordedAudioData.size()) "<<recordedAudioData.size();
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
        input_timer->start(10);
        // connect(inputDevice, &QIODevice::readyRead, this, &AudioHandler::onNotify);
    }
}

void AudioHandler::stopAudioInput()
{
    if (audioInputsource)
    {
        input_timer->stop();
        audioInputsource->stop();
    }
}

void AudioHandler::startAudioOutput()
{
    // qDebug() << "onstartListening";
    outputDevice->start();
    m_output = audioOutputsource->start();
    output_timer->start(10);
}

void AudioHandler::stopAudioOutput()
{
    qDebug() << "stopAudioOutput";
    output_timer->stop();
    outputDevice->stop();
    audioOutputsource->stop();
}
