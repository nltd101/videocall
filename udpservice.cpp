#include "udpservice.h"
#include "bigpackage.h"
std::mutex mtx;
void die(char* s)
{
    perror(s);
    exit(1);
}
int UdpService::getMyPort() {
    return ntohs(this->my_address.sin_port);
}


void UdpService::openPortAndListen()
{
    memset((char*)&this->my_address, 0, sizeof(this->my_address));
    this->my_address.sin_family = AF_INET;
    this->my_address.sin_port = htons(0);
    //htons(PORT);
    this->my_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port

    if (bind(this->socket, (struct sockaddr*)&this->my_address, sizeof(this->my_address)) == -1)
    {
        die("bind");
    }

    socklen_t len = sizeof(this->my_address);
    getsockname(this->socket, (struct sockaddr*)&this->my_address, &len);

    cout << "local port: %d" << this->getMyPort() << endl;
    char buf[BUFLEN];
    //keep listening for data
    struct sockaddr_in sender_address;
    socklen_t sender_len, recv_len;


    unique_ptr<BigPackage> frame_package = nullptr;
    unique_ptr<BigPackage> sound_package = nullptr;
    while (this->is_running)
    {
        // cout << "Waiting for data..." << endl;

        if ((recv_len = recvfrom(this->socket, buf, BUFLEN, 0, (struct sockaddr*)&sender_address, &sender_len)) == -1)
        {
            die("recvfrom()");
            // continue;
        }

        //print details of the client/peer and the data received
        // printf("Received packet from %s:%d\n", inet_ntoa(sender_address.sin_addr), ntohs(sender_address.sin_port));
        // printf("Data: %s\n", buf);
//        cout<<"port---------------------------"<<endl;
//        cout << ntohs(this->partner_address.sin_port) << endl;
//        cout << ntohs(sender_address.sin_port) << endl;
//          cout<<"address---------------------------"<<endl;
//          char str[30];
//          inet_ntop(AF_INET, &(this->partner_address.sin_addr), str, INET_ADDRSTRLEN);
//          cout << str << endl;
//          inet_ntop(AF_INET, &(sender_address.sin_addr), str, INET_ADDRSTRLEN);
//          cout << str << endl;
       // cout<<this->partner_address.sin_addr.s_addr<<endl;
        //  cout << sender_address.sin_addr.s_addr << endl;
//          cout<<"length---------------------------"<<endl;
//          cout << this->partner_addr_length<< endl;
//          cout << sender_len << endl;
        // cout << buf[BUFLEN] << endl;
        // cout << ntohs(this->partner_address.sin_port);



        if (ntohs(this->partner_address.sin_port) == 0) {
            this->partner_address = sender_address;
            this->partner_addr_length = sender_len;
        }
        if ((sender_address.sin_addr.s_addr == this->partner_address.sin_addr.s_addr
            && sender_address.sin_port == this->partner_address.sin_port
            && sender_len == this->partner_addr_length))
        {



            int flag = BigPackage::getFlag(buf);

            cout<<flag<<endl;
            switch (flag) {
            case BigPackage::FRAME:
                if (this->onReceiveFrame != NULL)
                {
                    //                    receive_data[full_receive_length] = '\0';
                    if (frame_package != nullptr)
                    {

                        this->onReceiveFrame(this->ui,frame_package->getCharData());
                        frame_package.reset();
                    }

                    frame_package = unique_ptr<BigPackage>(new BigPackage(buf, recv_len, BigPackage::FRAME));
                }
                break;
            case BigPackage::FRAME_PACKAGE:
                if (this->onReceiveFrame != NULL && frame_package != nullptr)
                {
                    frame_package->pushPackage(buf, recv_len);
                }

                break;
            case BigPackage::SOUND:
                if (this->onReceiveSound != NULL)
                {
                    if (sound_package != nullptr)
                    {

                        this->onReceiveSound(this->ui,sound_package->getCharData());
                        sound_package.reset();
                    }

                    sound_package = unique_ptr<BigPackage>(new BigPackage(buf, recv_len, BigPackage::SOUND));
                }
                break;
            case BigPackage::SOUND_PACKAGE:
                if (this->onReceiveSound != NULL && sound_package != nullptr)
                {
                    sound_package->pushPackage(buf, recv_len);
                }

                break;
            default:
                break;
            }
        }

    }
    if (frame_package != nullptr && this->onReceiveFrame != NULL)
    {
        this->onReceiveFrame(this->ui,frame_package->getCharData());
        frame_package.reset();
    }
    if (sound_package != nullptr && this->onReceiveSound != NULL)
    {
        this->onReceiveSound(this->ui,sound_package->getCharData());
        sound_package.reset();
    }
}

void UdpService::setReceiveFrameListener(void (*onReceiveFrame)(Ui::MainWindow* ui,char* data)) {
    this->onReceiveFrame = onReceiveFrame;
}
void UdpService::setReceiveSoundListener(void (*onReceiveSound)(Ui::MainWindow* ui,char* data)) {
    this->onReceiveSound = onReceiveSound;
}


void UdpService::sendFrame(char* data) {
    this->sendData(data, BigPackage::FRAME);
}
void UdpService::sendSound(char* data) {
    this->sendData(data, BigPackage::SOUND);
}

void UdpService::sendData(char* data, int flag) {

    if (ntohs(this->partner_address.sin_port) == 0)return;
    cout << "send size: " << strlen(data) << endl;
    unique_ptr<BigPackage> data_to_send = unique_ptr<BigPackage>(new BigPackage(data, flag));


    int package_amount = data_to_send->getAmountPackageToSend();

    for (int i = 0; i < package_amount; i++) {
        char* buf = data_to_send->getPackage(i);

        if (sendto(this->socket, buf, strlen(buf), 0, (struct sockaddr*)&this->partner_address, this->partner_addr_length) == -1)
        {
            die("sendto()");
        }
        delete buf;

    }


}


void UdpService::setPartnerAddress(char* ip, int port) {

    memset((char*)&this->partner_address, 0, sizeof(this->partner_addr_length));
    this->partner_address.sin_family = AF_INET;
    this->partner_address.sin_port = htons(port);

    cout << "partner's port:" << ntohs(this->partner_address.sin_port) << endl;
    if (inet_aton(ip, &this->partner_address.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
}
void UdpService::start() {
    this->is_running = true;
}
void UdpService::stop() {
    this->is_running = false;
}

UdpService::UdpService(Ui::MainWindow* ui)
{
    this->ui=ui;
    this->partner_addr_length = sizeof(partner_address);

    if ((this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    this->listenConnect = new thread(&UdpService::openPortAndListen, this);
    this->listenConnect->detach();
}
UdpService::UdpService(Ui::MainWindow* ui,char* ip, int port)
{
     this->ui=ui;
    setPartnerAddress(ip, port);
    this->partner_addr_length = sizeof(partner_address);

    if ((this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    this->listenConnect = new thread(&UdpService::openPortAndListen, this);
    this->listenConnect->detach();
}

UdpService::~UdpService() {

    delete this->listenConnect;

    close(this->socket);
}
bool UdpService::isRunning() {
    return this->is_running;
}
