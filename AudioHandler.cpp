#include "audiohandler.h"
#include <QDebug>
AudioHandler::AudioHandler(QObject *parent)
    :QObject(parent), audioInputsource(nullptr), audioOutputsource(nullptr)
    ,m_outputMediaDevices(new QMediaDevices(this)), m_inputTimer(new QTimer(this))
    , m_outputTimer(new QTimer(this)),m_pushTimer(new QTimer(this))
{
    for(int i = 0; i < QMediaDevices::audioInputs().count(); ++i)
    {
        auto aa = QMediaDevices::audioInputs().at(i);
        qDebug() << "音频输入：" << aa.description();
    }
    for(int i = 0; i < QMediaDevices::audioOutputs().count(); ++i)
    {
        auto aa = QMediaDevices::audioOutputs().at(i);
        qDebug() << "音频输出：" << aa.description();
    }

    initializeInputAudio(QMediaDevices::defaultAudioInput());
    //    initializeOutPutAudio(m_outputMediaDevices->audioOutputs().at(2));
    initializeOutPutAudio(m_outputMediaDevices->defaultAudioOutput());

    connect(m_inputTimer, &QTimer::timeout, this, &AudioHandler::onInputNotify);


    connect(inputDevice.data(), &AudioIputDevices::levelChanged, [=](qreal value){
        //        qDebug() << "emit signalAudioLevel(value);" << value;
        emit signalAudioLevel(value);
    });

    connect(m_outputTimer, &QTimer::timeout, this, &AudioHandler::onOutputNotify);

    onStopTalking();
}

AudioHandler::~AudioHandler()
{
    if(m_outputTimer)
    {
        m_outputTimer->deleteLater();
        m_outputTimer = nullptr;
    }
    if(m_inputTimer)
    {
        m_inputTimer->deleteLater();
        m_inputTimer = nullptr;
    }
}

void AudioHandler::initializeInputAudio(const QAudioDevice &deviceInfo)
{
    //设置录音的格式
    inputAudioFormat.setSampleRate(8000); //设置采样率以对赫兹采样。 以秒为单位，每秒采集多少声音数据的频率.
    inputAudioFormat.setChannelCount(1);   //将通道数设置为通道。
    // audioFormat.setSampleSize(16);     /*将样本大小设置为指定的sampleSize（以位为单位）通常为8或16，但是某些系统可能支持更大的样本量。*/
    inputAudioFormat.setSampleFormat(QAudioFormat::Int16);
    // audioFormat.setCodec("audio/pcm"); //设置编码格式
    // audioFormat.setByteOrder(QAudioFormat::LittleEndian); //样本是小端字节顺序
    // audioFormat.setSampleType(QAudioFormat::SignedInt); //样本类型

    // ChannelConfigStereo is 2, Int16 is 2
    qDebug("sampleRate: %d, channelCount: %d, sampleFormat: %d",
           inputAudioFormat.sampleRate(), inputAudioFormat.channelCount(), inputAudioFormat.sampleFormat()
           );

    inputDevice.reset(new AudioIputDevices(inputAudioFormat));

    audioInputsource.reset(new QAudioSource(deviceInfo, inputAudioFormat));

    connect(inputDevice.data(), &AudioIputDevices::signalInputAudioBytearrayData, [=](const char *data, qint64 len){
        qDebug() << "m_audioByteArrayData start:" << "m_audioByteArrayData size:" << m_audioByteArrayData.size() ;
        QByteArray aa(data, len);
        m_audioByteArrayData.append(aa);
        qDebug() << "m_audioByteArrayData final:" << "m_audioByteArrayData size:" << m_audioByteArrayData.size() ;
    });


}

void AudioHandler::initializeOutPutAudio(const QAudioDevice &deviceInfo)
{
    qDebug() << "outputAudioDevice音频输出：" << deviceInfo.description();

    outputAudioFormat = deviceInfo.preferredFormat();

    outputAudioFormat.setSampleRate(8000); //设置采样率以对赫兹采样。 以秒为单位，每秒采集多少声音数据的频率.
    outputAudioFormat.setChannelCount(1);   //将通道数设置为通道。
    // audioFormat.setSampleSize(16);     /*将样本大小设置为指定的sampleSize（以位为单位）通常为8或16，但是某些系统可能支持更大的样本量。*/
    outputAudioFormat.setSampleFormat(QAudioFormat::Int16);

    if(!deviceInfo.isFormatSupported(outputAudioFormat))
    {
        qWarning() << "not Support fromat";
    }

    qDebug("sampleRate: %d, channelCount: %d, sampleFormat: %d",
           outputAudioFormat.sampleRate(), outputAudioFormat.channelCount(), outputAudioFormat.sampleFormat()
           );

    const int durationSeconds = 1;
    const int toneSampleRateHz = 600;
    outputDevice.reset(new AudioOutputDevices(outputAudioFormat, durationSeconds * 1000000, toneSampleRateHz));
    audioOutputsource.reset(new QAudioSink(deviceInfo, outputAudioFormat));

}

void AudioHandler::processAudioData(const QByteArray &data)
{
    emit signalSendData(data);
}

void AudioHandler::onOutputNotify()
{
    if(m_audioOutputByteArryaData.size() == 0)
        return;
    qDebug() << "m_audioOutputByteArryaData大小:" << m_audioOutputByteArryaData.size();

    //        auto io = audioOutputsource->start();
    //        int len = audioOutputsource->bytesFree();
    //        qDebug() << "len:" << len;
    //        len = io->write(m_audioOutputByteArryaData.data(), m_audioOutputByteArryaData.size());

    int len = audioOutputsource->bytesFree();
    qDebug() << "len:" << len;
    len = m_output->write(m_audioOutputByteArryaData.data(), m_audioOutputByteArryaData.size());

    m_audioOutputByteArryaData.clear();

}

void AudioHandler::onInputNotify()
{
    // Read available audio input data and send it
    if (inputDevice)
    {
        //        QByteArray audioData = inputDevice->readAll();
        // qDebug() << "audioData" << audioData;
        // QFile file("output.pcm");
        // if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
        // {
        //     qDebug() << "unable to open file";
        // }
        //  qDebug() << "file.write";
        // file.write("aaaaaaaaa");
        // file.write(audioData.data(), audioData.size());
        // file.close();

        QByteArray adtsHeaders = m_dataHandle.createADTSHeader(m_audioByteArrayData.size() + 7);

        qDebug() << "onInputNotify" << "m_audioByteArrayData size:" << m_audioByteArrayData.size() ;

        processAudioData(m_audioByteArrayData);

        m_audioByteArrayData.clear();
    }

}


void AudioHandler::onStartTalking()
{
    qDebug() << "onStartTalking";
    stopAudioOutput();
    startAudioInput();
    emit signalRequestTalk(CMDTYPE::CMDTALK);

}

void AudioHandler::onStopTalking()
{
    qDebug() << "onStopTalking";
    stopAudioInput();
    startAudioOutput();
    emit signalRequestTalk(CMDTYPE::CMDSTOPTALK);
    // if(m_timer)
    // {
    //     m_timer->stop();
    //     delete m_timer;
    //     m_timer = nullptr;
    // }
}

void AudioHandler::onAudioDataformWebsocket(const QByteArray &audioOutputData)
{

    m_audioOutputByteArryaData.append(audioOutputData);
    qDebug() << "onAudioDataformWebsocket" << "m_audioByteArrayData size:" << m_audioOutputByteArryaData.size() ;
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
    //        QFile file("clip_0002.wav");

    //        audioOutputsource->start(&file);
}

void AudioHandler::stopAudioOutput()
{
    qDebug() << "onstopListening";
    m_outputTimer->stop();
    outputDevice->stop();
    audioOutputsource->stop();
}

// void AudioHandler::deviceChanged(QAudioDevice device, int index)
// {
//     outputDevice->stop();
//     audioOutputsource->stop();
//     audioOutputsource->disconnect(this);
//     initializeOutPutAudio(device);
// }
