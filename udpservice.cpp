#include "udpservice.h"
#include "bigpackage.h"

void die(char *s)
{
	mdebug(s);
}
int UdpService::getMyPort()
{
	return ntohs(_myAddress.sin_port);
}

void UdpService::_openPortAndListen()
{
	memset((char *)&_myAddress, 0, sizeof(_myAddress));
	_myAddress.sin_family = AF_INET;
	_myAddress.sin_port = htons(0);
	_myAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	// bind socket to port

	if (bind(_socket, (struct sockaddr *)&_myAddress, sizeof(_myAddress)) == -1)
	{
		die("bind");
	}

	socklen_t len = sizeof(_myAddress);
	getsockname(_socket, (struct sockaddr *)&_myAddress, &len);

	cout << "local port: %d" << getMyPort() << endl;
	unsigned char buf[BUFLEN];

	struct sockaddr_in senderAddress;
	socklen_t senderLen, recvLen;

	unique_ptr<BigPackage> framePackage = nullptr;
	unique_ptr<BigPackage> soundPackage = nullptr;
	while (true)
	{
		if (!_isRunning)
			continue;
		if ((recvLen = recvfrom(_socket, buf, BUFLEN, 0, (struct sockaddr *)&senderAddress, &senderLen)) == -1)
		{
			die("recvfrom()");
		}

		if (ntohs(_partnerAddress.sin_port) == 0)
		{
			_partnerAddress = senderAddress;
			_partnerAddrLength = senderLen;
		}
		if ((senderAddress.sin_addr.s_addr == _partnerAddress.sin_addr.s_addr && senderAddress.sin_port == _partnerAddress.sin_port && senderLen == _partnerAddrLength))
		{
			int flag = BigPackage::getFlag(buf);
			switch (flag)
			{
			case BigPackage::FRAME:
				if (_onReceiveFrame != NULL)
				{
					if (framePackage != nullptr)
					{
						unsigned char *data;
						int length;
						std::tie(data, length) = framePackage->getCharData();
						_onReceiveFrame(_frameParent, data, length);
						framePackage.reset();
					}

					framePackage = unique_ptr<BigPackage>(new BigPackage(buf, recvLen));
				}
				break;
			case BigPackage::FRAME_PACKAGE:
				if (_onReceiveFrame != NULL && framePackage != nullptr)
				{
					framePackage->pushPackage(buf, recvLen);
				}

				break;
			case BigPackage::SOUND:
				if (_onReceiveSound != NULL)
				{
					if (soundPackage != nullptr)
					{
						unsigned char *data;
						int length;
						std::tie(data, length) = soundPackage->getCharData();
						_onReceiveSound(_soundParent, data, length);
						soundPackage.reset();
					}

					soundPackage = unique_ptr<BigPackage>(new BigPackage(buf, recvLen));
				}
				break;
			case BigPackage::SOUND_PACKAGE:
				if (_onReceiveSound != NULL && soundPackage != nullptr)
				{
					soundPackage->pushPackage(buf, recvLen);
				}

				break;
			default:
				break;
			}
		}
	}
	if (framePackage != nullptr && _onReceiveFrame != NULL)
	{
		unsigned char *data;
		int length;
		std::tie(data, length) = framePackage->getCharData();
		_onReceiveFrame(_frameParent, data, length);
		framePackage.reset();
	}
	if (soundPackage != nullptr && _onReceiveSound != NULL)
	{
		unsigned char *data;
		int length;
		std::tie(data, length) = soundPackage->getCharData();
		_onReceiveSound(_soundParent, data, length);
		soundPackage.reset();
	}
}

void UdpService::setReceiveFrameListener(void *parent, void (*onReceiveFrame)(void *parent, unsigned char *data, int length))
{
	_frameParent = parent;
	_onReceiveFrame = onReceiveFrame;
}
void UdpService::setReceiveSoundListener(void *parent, void (*onReceiveSound)(void *parent, unsigned char *data, int length))
{
	_soundParent = parent;
	_onReceiveSound = onReceiveSound;
}

void UdpService::sendFrame(unsigned char *data, int length)
{
	_sendData(data, length, BigPackage::FRAME);
}
void UdpService::sendSound(unsigned char *data, int length)
{
	_sendData(data, length, BigPackage::SOUND);
}

void UdpService::_sendData(unsigned char *data, int length, int flag)
{
	if (_isRunning == false)
		return;
	if (ntohs(_partnerAddress.sin_port) == 0)
		return;
	_bytePerS += length;

	unique_ptr<BigPackage> dataToSend = unique_ptr<BigPackage>(new BigPackage(data, length, flag));

	int packageAmount = dataToSend->getAmountPackageToSend();

	for (int i = 0; i < packageAmount; i++)
	{
		unsigned char *data;
		int length;
		std::tie(data, length) = dataToSend->getPackageToSend(i);
		if (sendto(_socket, data, length, 0, (struct sockaddr *)&_partnerAddress, _partnerAddrLength) == -1)
		{
			die("sendto()");
		}
		delete data;
	}
}

void UdpService::setPartnerAddress(char *ip, int port)
{

	memset((char *)&_partnerAddress, 0, sizeof(_partnerAddrLength));
	_partnerAddress.sin_family = AF_INET;
	_partnerAddress.sin_port = htons(port);

	cout << "partner's port:" << ntohs(_partnerAddress.sin_port) << endl;
	if (inet_aton(ip, &_partnerAddress.sin_addr) == 0)
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
}
void UdpService::start()
{
	_isRunning = true;
}
void UdpService::stop()
{
	_isRunning = false;
}

UdpService::UdpService()
{
	_partnerAddrLength = sizeof(_partnerAddress);

	if ((_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
	_listenConnect = new thread(&UdpService::_openPortAndListen, this);
	_timmerThread = new thread(&UdpService::_startTimer, this);
}
void UdpService::_startTimer()
{
	// infinite loop because timer will keep
	// counting. To kill the process press
	// Ctrl+D. If it does not work ask
	// ubuntu for other ways.
	while (true)
	{

		// display the timer

		// sleep system call to sleep
		// for 1 second
		sleep(1);
		// kilo bit per second
		cout << _bytePerS * 8 / 1000 << "kbps" << endl;
		// increment seconds
		_bytePerS = 0;
	}
}
UdpService::UdpService(char *ip, int port)
{
	setPartnerAddress(ip, port);
	_partnerAddrLength = sizeof(_partnerAddress);

	if ((_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
	_listenConnect = new thread(&UdpService::_openPortAndListen, this);
	_timmerThread = new thread(&UdpService::_startTimer, this);
}

UdpService::~UdpService()
{
	_listenConnect->join();
	_timmerThread->join();

	if (_listenConnect != NULL)
	{

		delete _listenConnect;
	}
	delete _timmerThread;
	close(_socket);
}
