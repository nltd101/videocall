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
  UdpService(char* ip, int port);

  ~UdpService();
  void setPartnerAddress(char* ip, int port);
  int getMyPort();
  void start();
  void stop();
  void openPortAndListen();
  void setReceiveFrameListener(void*, void (*onReceiveFrame)(void* parent, unsigned char* data, int));
  void setReceiveSoundListener(void*, void (*onReceiveSound)(void* parent, unsigned char* data, int));
  void sendFrame(unsigned char* data, int);
  void sendSound(unsigned char* data, int);

private:
  void sendData(unsigned char* data, int, int flag);
  void startTimer();
  void (*onReceiveFrame)(void* parent, unsigned char* data, int length);
  void (*onReceiveSound)(void* parent, unsigned char* data, int length);
  int bytePerS;
  socklen_t socket;
  struct sockaddr_in partnerAddress, myAddress;
  socklen_t partnerAddrLength, myAddrLength;
  thread* listenConnect = NULL;
  void* frameParent;
  void* soundParent;
  bool isRunning;
  bool isSending;
  thread* timmerThread;
};

#endif // UDPSERVICE_H
