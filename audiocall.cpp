#include "audiocall.h"
#include <mutex>
mutex a;
void AudioCall::onRetriveSound(void* parrent, char* data, int length){
   AudioCall*  displayer = static_cast<AudioCall *> (parrent);

     displayer->sound(data,length);

}
void sound2(QIODevice* io, char*data, int length){


    QByteArray *byteBuffer = new QByteArray(data, strlen(data));

    QBuffer *input = new QBuffer(byteBuffer);
    input->open(QIODevice::ReadOnly); // set the QIODevice to read-only
    io->write(*byteBuffer);
    input->close();
    delete input;
}

AudioCall::AudioCall(QMainWindow *context, UdpService *udp_service)
{
    this->context = context;
    this->udp_service = udp_service;
    QAudioFormat format;

    format.setSampleRate(8000);
    format.setChannelCount(1);

    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::BigEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    QAudioDeviceInfo inputinfor = QAudioDeviceInfo::defaultInputDevice();
    if (!inputinfor.isFormatSupported(format))
    {
        qWarning() << "Default format not supported, trying to use the nearest.";

        inputaudio = new QAudioInput(inputinfor.nearestFormat(format), context);
    }
    else
    {
        inputaudio = new QAudioInput(format, context);
    }
    QAudioDeviceInfo outputinfor = QAudioDeviceInfo::defaultOutputDevice();
    if (!outputinfor.isFormatSupported(format))
    {
        qWarning() << "Default format not supported, trying to use the nearest.";

        outputaudio = new QAudioOutput(outputinfor.nearestFormat(format), context);
    }
    else
    {

        outputaudio = new QAudioOutput(format, context);
    }
    input_device = inputaudio->start();
    output_device = outputaudio->start();
    auto io = input_device;
    QObject::connect(io, &QIODevice::readyRead, [this, io]  {
                QByteArray buff = input_device->readAll();
               int buffLenght = buff.length();
                if (buffLenght > 0)
                {
                    char *data = (char *)buff.constData();

                   this->udp_service->sendSound(data,buffLenght);

                } });
    this->udp_service->setReceiveSoundListener(this,&AudioCall::onRetriveSound);
}

void AudioCall::sound(char *data, int length)
{
a.lock();
    QByteArray *byteBuffer = new QByteArray(data, length);
    QBuffer *input = new QBuffer(byteBuffer);
    input->open(QIODevice::ReadOnly);
    output_device->write(*byteBuffer);
    input->close();
    delete input;
a.unlock();
}
void AudioCall::handleStateChanged(QAudio::State newState)
{
    switch (newState)
    {
    case QAudio::StoppedState:
        if (inputaudio->error() != QAudio::NoError)
        {
            // Error handling
        }
        else
        {
            // Finished recording
        }
        break;

    case QAudio::ActiveState:

        // Started recording - read from IO device
        break;

    default:
        // ... other cases as appropriate
        break;
    }
}
