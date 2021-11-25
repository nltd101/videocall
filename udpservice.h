#ifndef UDPSERVICE_H
#define UDPSERVICE_H

#define DEBUG_MODE true
#define mdebug(x) do { \
  if (DEBUG_MODE) { std::cerr << x << std::endl; } \
} while (0)
#define mlog(x) do { \
  if (DEBUG_MODE) { std::cout << x << std::endl; } \
} while (0)
#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <mutex>
#define BUFLEN 64000

namespace Ui
{
    class MainWindow;
}


using namespace std;

class UdpService
{
public:
    UdpService(Ui::MainWindow *ui);
    UdpService(Ui::MainWindow *ui, char* ip, int port);

    ~UdpService();
    void (*onReceiver)(Ui::MainWindow *ui, char* data);
    void send(char* data);

    void setPartnerAddress(char* ip, int port);
    int getMyPort();
    char* getMyIp();
    void start();
    void stop();
    void openPortAndListen();
    bool isRunning();
private:
    socklen_t socket;
    struct sockaddr_in partner_address, my_address;
    socklen_t partner_addr_length, my_addr_length;
    thread * listenConnect=NULL;
    Ui::MainWindow *ui;
    int receiveNums(char*,int);
    void receiveData(char*,int,int&,char*);
    bool is_running;
    bool is_sending;
    void setIsSendingFalse();
};

#endif // UDPSERVICE_H
