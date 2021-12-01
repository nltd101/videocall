#ifndef UDPSERVICE_H
#define UDPSERVICE_H

#define DEBUG_MODE true
#define mdebug(x)                  \
  do                               \
  {                                \
    if (DEBUG_MODE)                \
    {                              \
      std::cerr << x << std::endl; \
    }                              \
  } while (0)
#define mlog(x)                    \
  do                               \
  {                                \
    if (DEBUG_MODE)                \
    {                              \
      std::cout << x << std::endl; \
    }                              \
  } while (0)

#ifndef BUFLEN
#define BUFLEN 64000
#endif
#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <memory>
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
  bool isRunning();
  void setReceiveFrameListener(void*, void (*onReceiveFrame)(void* parent, char* data, int));
  void setReceiveSoundListener(void*, void (*onReceiveSound)(void* parent, char* data, int));
  void sendFrame(char* data, int);
  void sendSound(char* data, int);

private:
  void (*onReceiveFrame)(void* parent, char* data, int length);
  void (*onReceiveSound)(void* parent, char* data, int length);
  socklen_t socket;
  struct sockaddr_in partner_address, my_address;
  socklen_t partner_addr_length, my_addr_length;
  thread* listenConnect = NULL;
  void* frame_parent;
  void* sound_parent;
  void sendData(char* data, int, int flag);
  bool is_running;
  bool is_sending;
  void setIsSendingFalse();
};

#endif // UDPSERVICE_H
