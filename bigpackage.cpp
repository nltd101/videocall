#include <string>
#include <cstring>
#include "bigpackage.h"
#include "iostream"
using namespace std;
BigPackage::BigPackage()
{

}
char* BigPackage::getCharData() {

    return this->data;
}
BigPackage::~BigPackage() {
    if (this->is_inited)
        delete this->data;
}

BigPackage::BigPackage(char* amount, int length, int flag)
{
    int package_nums = 0;
    for (int i = 1; i < length; i++) {
        package_nums = package_nums * 10 + (amount[i] - '0');
    }
    this->p_amount = package_nums;
    this->data = new char[p_amount * BUFLEN + 1];
    this->is_inited = true;
    this->flag = flag;
    this->full_length = 0;
}

char BigPackage::getFlag(char* amount)
{
    return amount[0];
}
BigPackage::BigPackage(char* data, int flag)
{

    this->flag = flag;
    this->data = data;

    this->full_length = strlen(data);

    int data_len = BUFLEN - 1;
    this->p_amount = strlen(data) / data_len + (strlen(data) % data_len == 0 ? 0 : 1);
    this->is_inited = false;
}
void BigPackage::pushPackage(char* buf, int length) {
    if (BigPackage::getFlag(buf) != BigPackage::getPackageFlag(this->flag)) return;
    for (int i = 1; i < length; i++) {
        this->data[this->full_length] = buf[i];
        this->full_length++;
    }

}
int BigPackage::getPackageFlag(int flag) {
    switch (flag)
    {
    case BigPackage::FRAME:
        return FRAME_PACKAGE;
    case BigPackage::SOUND:
        return SOUND_PACKAGE;
        break;

    default:
        return -1;
        break;
    }
    return -1;
}
char* BigPackage::getPackage(int index) {
    if (index == 0)
    {
        int package_amount = this->p_amount;
        std::string tmp = std::to_string(package_amount);
        char const* num_char = tmp.c_str();
        char* buf = new char[BUFLEN];

        buf[0] = this->flag;
        buf[1] = '\0';
        strcat(buf, num_char);
        buf[strlen(buf)] = '\0';
        return buf;
    }
    index--;
    if (index * (BUFLEN - 1) >= this->full_length)
    {
        return nullptr;
    }
    int data_len = BUFLEN - 1;
    int begin = index * data_len;
    int len = std::min((int)(strlen(data) - index * data_len), (int)data_len);

    char* res = new char[len + 1];

    res[0] = getPackageFlag(this->flag);
    for (int i = 0; i < len; i++)
        res[i + 1] = *(this->data + begin + i);
    res[len + 1] = '\0';
    return res;
}
int BigPackage::getAmountPackageToSend() {
    return this->p_amount + 1;
}
