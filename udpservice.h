#ifndef UDPSERVICE_H
#define UDPSERVICE_H

#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <memory>
#include "debugmarco.h"
namespace Ui
{
	class MainWindow;
}

using namespace std;

class UdpService
{
public:
	UdpService();
	UdpService(char *ip, int port);
	~UdpService();

	void setPartnerAddress(char *ip, int port);
	int getMyPort();
	void start();
	void stop();

	void setReceiveFrameListener(void *, void (*onReceiveFrame)(void *parent, unsigned char *data, int));
	void setReceiveSoundListener(void *, void (*onReceiveSound)(void *parent, unsigned char *data, int));
	void sendFrame(unsigned char *data, int);
	void sendSound(unsigned char *data, int);

private:
	void _openPortAndListen();
	void _sendData(unsigned char *data, int, int flag);
	void _startTimer();
	void (*_onReceiveFrame)(void *parent, unsigned char *data, int length);
	void (*_onReceiveSound)(void *parent, unsigned char *data, int length);
	int _bytePerS;
	socklen_t _socket;
	struct sockaddr_in _partnerAddress, _myAddress;
	socklen_t _partnerAddrLength, _myAddrLength;
	thread *_listenConnect = NULL;
	void *_frameParent;
	void *_soundParent;
	bool _isRunning;
	bool _isSending;
	thread *_timmerThread;
};

#endif // UDPSERVICE_H
