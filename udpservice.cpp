#include "udpservice.h"


void die(char* s)
{
    perror(s);
    exit(1);
}
int UdpService::getMyPort() {
    return ntohs(this->my_address.sin_port);
}
char* UdpService::getMyIp() {
    if (this->my_address.sin_port == 0) {
        return NULL;
    }
    char myIP[16];
    inet_ntop(AF_INET, &this->my_address.sin_addr, myIP, sizeof(myIP));
    return myIP;
    // printf("local address: %sn", inet_ntoa( this->my_address.sin_addr));
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

    printf("local port: %d", this->getMyPort());

    char buf[BUFLEN];
    //keep listening for data
    struct sockaddr_in sender_address;
    socklen_t sender_len, recv_len;
    while (1)
    {
        cout << "Waiting for data..." << endl;
        // fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(this->socket, buf, BUFLEN, 0, (struct sockaddr*)&sender_address, &sender_len)) == -1)
        {
            die("recvfrom()");
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(sender_address.sin_addr), ntohs(sender_address.sin_port));
        printf("Data: %s\n", buf);
        cout << ntohs(this->partner_address.sin_port);
        if (this->partner_address.sin_port == 0) {
            this->partner_address = sender_address;
            this->partner_addr_length = sender_len;
        }
        if ((this->partner_address.sin_port == 0) || (sender_address.sin_addr.s_addr == this->partner_address.sin_addr.s_addr && sender_address.sin_port == this->partner_address.sin_port && sender_len == this->partner_addr_length))
        {
            if (this->onReceiver)
                this->onReceiver(this->ui,buf);
        }
    }
}
void UdpService::send(char* data) {
    if (ntohs(this->partner_address.sin_port) == 0)return;
    //now reply the client with the same data
    cout << "send to " << ntohs(this->partner_address.sin_port) << endl;
    if (sendto(this->socket, data, sizeof(data), 0, (struct sockaddr*)&this->partner_address, this->partner_addr_length) == -1)
    {
        die("sendto()");
    }
}

UdpService::UdpService(Ui::MainWindow *ui,char* ip, int port)
{
    this->ui=ui;
    setPartnerAddress(ip, port);

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

void UdpService::setUp()
{
    this->partner_addr_length = sizeof(partner_address);

    if ((this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    this->listenConnect = new thread(&UdpService::openPortAndListen, this);
    // thread listenConnect(&UdpCallService::openPortAndListen, this);
//    while (1)
//    {
//        char s[BUFLEN];
//        cout << "type the message:";
//        cin >> s;
//        this->send(s);
//        /* code */
//    }



}
UdpService::UdpService(Ui::MainWindow *ui)
{
    this->ui=ui;
}

UdpService:: ~UdpService() {
    this->listenConnect->join();
    delete this->listenConnect;
    close(this->socket);
}
