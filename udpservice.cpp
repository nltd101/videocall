#include "udpservice.h"
#include "bigpackage.h"
std::mutex mtx;
void die(char *s)
{
    perror(s);
    exit(1);
}
int UdpService::getMyPort()
{
    return ntohs(this->my_address.sin_port);
}

void UdpService::openPortAndListen()
{
    memset((char *)&this->my_address, 0, sizeof(this->my_address));
    this->my_address.sin_family = AF_INET;
    this->my_address.sin_port = htons(0);
    this->my_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to port

    if (bind(this->socket, (struct sockaddr *)&this->my_address, sizeof(this->my_address)) == -1)
    {
        die("bind");
    }

    socklen_t len = sizeof(this->my_address);
    getsockname(this->socket, (struct sockaddr *)&this->my_address, &len);

    cout << "local port: %d" << this->getMyPort() << endl;
    char buf[BUFLEN];

    struct sockaddr_in sender_address;
    socklen_t sender_len, recv_len;

    unique_ptr<BigPackage> frame_package = nullptr;
    unique_ptr<BigPackage> sound_package = nullptr;
    while (this->is_running)
    {

        if ((recv_len = recvfrom(this->socket, buf, BUFLEN, 0, (struct sockaddr *)&sender_address, &sender_len)) == -1)
        {
            die("recvfrom()");
        }

        if (ntohs(this->partner_address.sin_port) == 0)
        {
            this->partner_address = sender_address;
            this->partner_addr_length = sender_len;
        }
        if ((sender_address.sin_addr.s_addr == this->partner_address.sin_addr.s_addr && sender_address.sin_port == this->partner_address.sin_port && sender_len == this->partner_addr_length))
        {

            int flag = BigPackage::getFlag(buf);

            switch (flag)
            {
            case BigPackage::FRAME:
                if (this->onReceiveFrame != NULL)
                {
                    if (frame_package != nullptr)
                    {
                        char *data;
                        int length;
                        std::tie(data, length) = frame_package->getCharData();
                        this->onReceiveFrame(this->frame_parent, data, length);
                        frame_package.reset();
                    }
                    int package_nums = 0;
                    for (int i = 1; i < recv_len; i++)
                    {
                        package_nums = package_nums * 10 + (buf[i] - '0');
                    }
                    frame_package = unique_ptr<BigPackage>(new BigPackage(package_nums, BigPackage::FRAME));
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
                        char *data;
                        int length;
                        std::tie(data, length) = sound_package->getCharData();
                        this->onReceiveSound(this->sound_parent, data, length);
                        sound_package.reset();
                    }
                    int package_nums = 0;
                    for (int i = 1; i < recv_len; i++)
                    {
                        package_nums = package_nums * 10 + (buf[i] - '0');
                    }
                    sound_package = unique_ptr<BigPackage>(new BigPackage(package_nums, BigPackage::SOUND));
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
        char *data;
        int length;
        std::tie(data, length) = frame_package->getCharData();
        this->onReceiveFrame(this->frame_parent, data, length);
        frame_package.reset();
    }
    if (sound_package != nullptr && this->onReceiveSound != NULL)
    {
        char *data;
        int length;
        std::tie(data, length) = frame_package->getCharData();
        this->onReceiveSound(this->sound_parent, data, length);
        sound_package.reset();
    }
}

void UdpService::setReceiveFrameListener(void *parent, void (*onReceiveFrame)(void *parent, char *data, int length))
{
    this->frame_parent = parent;
    this->onReceiveFrame = onReceiveFrame;
}
void UdpService::setReceiveSoundListener(void *parent, void (*onReceiveSound)(void *parent, char *data, int length))
{
    this->sound_parent = parent;
    this->onReceiveSound = onReceiveSound;
}

void UdpService::sendFrame(char *data, int length)
{
    this->sendData(data, length, BigPackage::FRAME);
}
void UdpService::sendSound(char *data, int length)
{
   // this->sendData(data, length, BigPackage::SOUND);
}

void UdpService::sendData(char *data, int length, int flag)
{
    if (this->is_running == false)
        return;
    if (ntohs(this->partner_address.sin_port) == 0)
        return;
    cout<<"length send"<<length<<endl;
    unique_ptr<BigPackage> data_to_send = unique_ptr<BigPackage>(new BigPackage(data, length, flag));

    int package_amount = data_to_send->getAmountPackageToSend();

    for (int i = 0; i < package_amount; i++)
    {

        char *data;
        int length;
        std::tie(data, length) = data_to_send->getPackage(i);

        if (sendto(this->socket, data, length, 0, (struct sockaddr *)&this->partner_address, this->partner_addr_length) == -1)
        {
            die("sendto()");
        }
        delete data;
    }
}

void UdpService::setPartnerAddress(char *ip, int port)
{

    memset((char *)&this->partner_address, 0, sizeof(this->partner_addr_length));
    this->partner_address.sin_family = AF_INET;
    this->partner_address.sin_port = htons(port);

    cout << "partner's port:" << ntohs(this->partner_address.sin_port) << endl;
    if (inet_aton(ip, &this->partner_address.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
}
void UdpService::start()
{
    this->is_running = true;
}
void UdpService::stop()
{
    this->is_running = false;
}

UdpService::UdpService()
{
    this->partner_addr_length = sizeof(partner_address);

    if ((this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    this->listenConnect = new thread(&UdpService::openPortAndListen, this);
    //this->listenConnect->detach();
}
UdpService::UdpService(char *ip, int port)
{
    setPartnerAddress(ip, port);
    this->partner_addr_length = sizeof(partner_address);

    if ((this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    this->listenConnect = new thread(&UdpService::openPortAndListen, this);

}

UdpService::~UdpService()
{


    if (this->listenConnect!=NULL)
    {
         this->listenConnect->join();
         delete this->listenConnect;
    }


    close(this->socket);
}
bool UdpService::isRunning()
{
    return this->is_running;
}
