#include "udpservice.h"
#include "bigpackage.h"

void die(char* s)
{
    mdebug(s);
}
int UdpService::getMyPort()
{
    return ntohs(this->myAddress.sin_port);
}

void UdpService::openPortAndListen()
{
    memset((char*)&this->myAddress, 0, sizeof(this->myAddress));
    this->myAddress.sin_family = AF_INET;
    this->myAddress.sin_port = htons(0);
    this->myAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to port

    if (bind(this->socket, (struct sockaddr*)&this->myAddress, sizeof(this->myAddress)) == -1)
    {
        die("bind");
    }

    socklen_t len = sizeof(this->myAddress);
    getsockname(this->socket, (struct sockaddr*)&this->myAddress, &len);

    cout << "local port: %d" << this->getMyPort() << endl;
    unsigned char buf[BUFLEN];

    struct sockaddr_in senderAddress;
    socklen_t senderLen, recvLen;

    unique_ptr<BigPackage> framePackage = nullptr;
    unique_ptr<BigPackage> soundPackage = nullptr;
    while (true)
    {
        if (!isRunning) continue;
        if ((recvLen = recvfrom(this->socket, buf, BUFLEN, 0, (struct sockaddr*)&senderAddress, &senderLen)) == -1)
        {
            die("recvfrom()");
        }

        if (ntohs(this->partnerAddress.sin_port) == 0)
        {
            this->partnerAddress = senderAddress;
            this->partnerAddrLength = senderLen;
        }
        if ((senderAddress.sin_addr.s_addr == this->partnerAddress.sin_addr.s_addr && senderAddress.sin_port == this->partnerAddress.sin_port && senderLen == this->partnerAddrLength))
        {
            int flag = BigPackage::getFlag(buf);
            switch (flag)
            {
            case BigPackage::FRAME:
                if (this->onReceiveFrame != NULL)
                {
                    if (framePackage != nullptr)
                    {
                        unsigned char* data;
                        int length;
                        std::tie(data, length) = framePackage->getCharData();
                        this->onReceiveFrame(this->frameParent, data, length);
                        framePackage.reset();
                    }

                    framePackage = unique_ptr<BigPackage>(new BigPackage(buf, recvLen));
                }
                break;
            case BigPackage::FRAME_PACKAGE:
                if (this->onReceiveFrame != NULL && framePackage != nullptr)
                {
                    framePackage->pushPackage(buf, recvLen);
                }

                break;
            case BigPackage::SOUND:
                if (this->onReceiveSound != NULL)
                {
                    if (soundPackage != nullptr)
                    {
                        unsigned char* data;
                        int length;
                        std::tie(data, length) = soundPackage->getCharData();
                        this->onReceiveSound(this->soundParent, data, length);
                        soundPackage.reset();
                    }

                    soundPackage = unique_ptr<BigPackage>(new BigPackage(buf, recvLen));
                }
                break;
            case BigPackage::SOUND_PACKAGE:
                if (this->onReceiveSound != NULL && soundPackage != nullptr)
                {
                    soundPackage->pushPackage(buf, recvLen);
                }

                break;
            default:
                break;
            }
        }
    }
    if (framePackage != nullptr && this->onReceiveFrame != NULL)
    {
        unsigned char* data;
        int length;
        std::tie(data, length) = framePackage->getCharData();
        this->onReceiveFrame(this->frameParent, data, length);
        framePackage.reset();
    }
    if (soundPackage != nullptr && this->onReceiveSound != NULL)
    {
        unsigned char* data;
        int length;
        std::tie(data, length) = soundPackage->getCharData();
        this->onReceiveSound(this->soundParent, data, length);
        soundPackage.reset();
    }
}

void UdpService::setReceiveFrameListener(void* parent, void (*onReceiveFrame)(void* parent, unsigned char* data, int length))
{
    this->frameParent = parent;
    this->onReceiveFrame = onReceiveFrame;
}
void UdpService::setReceiveSoundListener(void* parent, void (*onReceiveSound)(void* parent, unsigned char* data, int length))
{
    this->soundParent = parent;
    this->onReceiveSound = onReceiveSound;
}

void UdpService::sendFrame(unsigned char* data, int length)
{
    this->sendData(data, length, BigPackage::FRAME);
}
void UdpService::sendSound(unsigned char* data, int length)
{
    this->sendData(data, length, BigPackage::SOUND);
}

void UdpService::sendData(unsigned char* data, int length, int flag)
{
    if (this->isRunning == false)
        return;
    if (ntohs(this->partnerAddress.sin_port) == 0)
        return;
    bytePerS += length;

    unique_ptr<BigPackage> dataToSend = unique_ptr<BigPackage>(new BigPackage(data, length, flag));

    int packageAmount = dataToSend->getAmountPackageToSend();

    for (int i = 0; i < packageAmount; i++)
    {
        unsigned char* data;
        int length;
        std::tie(data, length) = dataToSend->getPackageToSend(i);
        if (sendto(this->socket, data, length, 0, (struct sockaddr*)&this->partnerAddress, this->partnerAddrLength) == -1)
        {
            die("sendto()");
        }
        delete data;
    }
}

void UdpService::setPartnerAddress(char* ip, int port)
{

    memset((char*)&this->partnerAddress, 0, sizeof(this->partnerAddrLength));
    this->partnerAddress.sin_family = AF_INET;
    this->partnerAddress.sin_port = htons(port);

    cout << "partner's port:" << ntohs(this->partnerAddress.sin_port) << endl;
    if (inet_aton(ip, &this->partnerAddress.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
}
void UdpService::start()
{
    this->isRunning = true;
}
void UdpService::stop()
{
    this->isRunning = false;
}

UdpService::UdpService()
{
    this->partnerAddrLength = sizeof(partnerAddress);

    if ((this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    this->listenConnect = new thread(&UdpService::openPortAndListen, this);
        this->timmerThread = new thread(&UdpService::startTimer, this);
}
void UdpService::startTimer()
{
    // infinite loop because timer will keep
    // counting. To kill the process press
    // Ctrl+D. If it does not work ask
    // ubuntu for other ways.
    while (true) {

        // display the timer

        // sleep system call to sleep
        // for 1 second
        sleep(1);
        // kilo bit per second
        cout << bytePerS * 8 / 1000 << "kbps" << endl;
        // increment seconds
        bytePerS = 0;
    }
}
UdpService::UdpService(char* ip, int port)
{
    setPartnerAddress(ip, port);
    this->partnerAddrLength = sizeof(partnerAddress);

    if ((this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    this->listenConnect = new thread(&UdpService::openPortAndListen, this);
       this->timmerThread = new thread(&UdpService::startTimer, this);
}

UdpService::~UdpService()
{
    this->listenConnect->join();
    this->timmerThread->join();

    if (this->listenConnect != NULL)
    {

        delete this->listenConnect;
    }
    delete this->timmerThread;
    close(this->socket);
}

