#include "audiocall.h"
#include <mutex>

void AudioCall::onRetriveSound(void *parrent, unsigned char *data, int length)
{
	AudioCall *displayer = static_cast<AudioCall *>(parrent);

	displayer->sound(data, length);
}
AudioCall::~AudioCall()
{
	delete this->_inputAudio;
	delete this->_outputAudio;
}

AudioCall::AudioCall(QMainWindow *context, UdpService *udpService)
{
	this->_context = context;
	this->_udpService = udpService;
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

		_inputAudio = new QAudioInput(inputinfor.nearestFormat(format), context);
	}
	else
	{
		_inputAudio = new QAudioInput(format, context);
	}
	QAudioDeviceInfo outputinfor = QAudioDeviceInfo::defaultOutputDevice();
	if (!outputinfor.isFormatSupported(format))
	{
		qWarning() << "Default format not supported, trying to use the nearest.";

		_outputAudio = new QAudioOutput(outputinfor.nearestFormat(format), context);
	}
	else
	{
		_outputAudio = new QAudioOutput(format, context);
	}
	_inputDevice = _inputAudio->start();
	_outputDevice = _outputAudio->start();
	auto io = _inputDevice;
	QObject::connect(io, &QIODevice::readyRead, [this, io]
					 {
						QByteArray buff = _inputDevice->readAll();
						int buffLenght = buff.length();
						if (buffLenght > 0)
						{
							unsigned char *data = (unsigned char *)buff.constData();
							_udpService->sendSound(data,buffLenght);
					} });
	this->_udpService->setReceiveSoundListener(this, &AudioCall::onRetriveSound);
}

void AudioCall::sound(unsigned char *data, int length)
{

	QByteArray *byteBuffer = new QByteArray((char *)data, length);
	QBuffer *input = new QBuffer(byteBuffer);
	input->open(QIODevice::ReadOnly);
	_outputDevice->write(*byteBuffer);
	input->close();
	delete input;
}
