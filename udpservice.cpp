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
    char* receive_data = NULL;
    int full_receive_length = 0;
    int package_nums = 0;
    // receive_data = new char[package_nums * BUFLEN + 1];
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
        // cout << ntohs(this->partner_address.sin_port) << endl;
        // cout << ntohs(sender_address.sin_port) << endl;
        // cout << buf[BUFLEN] << endl;
        // cout << ntohs(this->partner_address.sin_port);
        // cout << "receive" << recv_len << endl;
        if (this->partner_address.sin_port == 0) {
            this->partner_address = sender_address;
            this->partner_addr_length = sender_len;
        }
        if (1 || (this->partner_address.sin_port == 0)
            || (sender_address.sin_addr.s_addr == this->partner_address.sin_addr.s_addr
                && sender_address.sin_port == this->partner_address.sin_port
                && sender_len == this->partner_addr_length))
        {
            if (this->onReceiver != NULL)
            {

                if (buf[0] == 'n') {

                    if (receive_data != NULL)
                    {
                         cout << "recieve data: " << strlen(receive_data) << endl;
                        receive_data[full_receive_length] = '\0';
                        this->onReceiver(this->ui,receive_data);
                    }
                    full_receive_length = 0;


                    package_nums = this->receiveNums(buf, recv_len);
                    // cout << "package nums: " << package_nums << endl;
                    if (receive_data != NULL)
                    {
                        // cout << "delete" << endl;
                        delete[] receive_data;
                    }

                    receive_data = new char[package_nums * BUFLEN + 1];

                }
                if (buf[0] == 'p') {
                    this->receiveData(buf, recv_len, full_receive_length, receive_data);
                }

            }

        }
    }
    if (receive_data != NULL)
    {
        receive_data[full_receive_length] = '\0';
        this->onReceiver(this->ui,receive_data);
        delete[] receive_data;
    }
}

int UdpService::receiveNums(char* buf, int length) {
    int package_nums = 0;
    for (int i = 1; i < length; i++) {
        package_nums = package_nums * 10 + (buf[i] - '0');

    }

    return package_nums;
}
void UdpService::receiveData(char* buf, int length, int& full_receive_length, char* receive_data) {
    int i = 1;


    for (int i = 1; i < length; i++) {
        receive_data[full_receive_length] = buf[i];
        full_receive_length++;
        // package_nums = package_nums * 10 + (buf[i] - '0');

    }
    // while (buf[i] != '\0') {

    //     i++;
    //     full_receive_length++;
    // }

}


char* splitToPackage(char* arr, int begin, int len)
{
    char* res = new char[len + 2];
    res[0] = 'p';
    for (int i = 0; i < len + 1; i++)
        res[i + 1] = *(arr + begin + i);
    res[len + 1] = 0;
    return res;
}
void UdpService::send(char* data) {
    if (ntohs(this->partner_address.sin_port) == 0)return;
    // data[strlen(data) + 1] = '\0';
    int data_len = BUFLEN - 1;
    int package_amount = strlen(data) / data_len + (strlen(data) % data_len == 0 ? 0 : 1);

    string tmp = std::to_string(package_amount);
    char const* num_char = tmp.c_str();
    char buf[BUFLEN] = "n";
    strcat(buf, num_char);
    buf[strlen(buf)]=='\0';
    cout<<strlen(buf)<<endl;

    // delete num_char;
    if (sendto(this->socket, buf, strlen(buf), 0, (struct sockaddr*)&this->partner_address, this->partner_addr_length) == -1)
    {
        die("sendto()");
    }
//    cout<<"size"<<buf<<endl;
    for (int i = 0; i < package_amount; i++) {
        char* buf = splitToPackage(data, i * data_len, min((int)(strlen(data) - i * data_len), (int)data_len));

//        cout << "send" << strlen(buf) << endl;
        if (sendto(this->socket, buf, strlen(buf), 0, (struct sockaddr*)&this->partner_address, this->partner_addr_length) == -1)
        {
            die("sendto()");
        }
        delete buf;
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

void UdpService::start()
{
    this->is_running=true;
    this->partner_addr_length = sizeof(partner_address);

    if ((this->socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    this->listenConnect = new thread(&UdpService::openPortAndListen, this);
}
UdpService::UdpService(Ui::MainWindow *ui)
{
    this->ui=ui;
}
bool UdpService::isRunning()
{
    return this->is_running;
}

UdpService:: ~UdpService() {
    this->listenConnect->join();
    delete this->listenConnect;
    close(this->socket);
}
