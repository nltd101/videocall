#include <string>
#include <cstring>
#include "bigpackage.h"
#include "iostream"
using namespace std;
BigPackage::BigPackage()
{
}
std::tuple<char*,int> BigPackage::getCharData()
{
    return {this->data,this->full_length};
}
BigPackage::~BigPackage()
{
    if (this->is_inited)
        delete this->data;
}

BigPackage::BigPackage(int package_nums, int flag)
{

    this->p_amount = package_nums;
    this->data = new char[p_amount * BUFLEN + 1];
    this->is_inited = true;
    this->flag = flag;
    this->full_length = 0;
}

int BigPackage::getFullLength()
{
    return this->full_length;
}

char BigPackage::getFlag(char *amount)
{
    return amount[0];
}
BigPackage::BigPackage(char *data, int length, int flag)
{

    this->flag = flag;
    this->data = data;
    this->full_length = length;
    int data_len = BUFLEN - 1;
    this->p_amount = length / data_len + (length % data_len == 0 ? 0 : 1);
    this->is_inited = false;
}
void BigPackage::pushPackage(char *buf, int length)
{
    if (BigPackage::getFlag(buf) != BigPackage::getPackageFlag(this->flag))
        return;
    for (int i = 1; i < length; i++)
    {
        this->data[this->full_length] = buf[i];
        this->full_length++;
    }
    this->last_length = length;
}
int BigPackage::getPackageFlag(int flag)
{
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
std::tuple<char*, int> BigPackage::getPackage(int index)
{
    if (index == 0)
    {
        int package_amount = this->p_amount;
        std::string tmp = std::to_string(package_amount);
        char const *num_char = tmp.c_str();
        char *buf = new char[BUFLEN];

        buf[0] = this->flag;
        buf[1] = '\0';
        strcat(buf, num_char);
        buf[strlen(buf)] = '\0';
        return {buf,strlen(buf)};
    }
    index--;
    if (index * (BUFLEN - 1) >= this->full_length)
    {
        return {nullptr,0};
    }
    int data_len = BUFLEN - 1;
    int begin = index * data_len;
    int len = std::min((int)(this->full_length - index * data_len), (int)data_len);

    char *res = new char[len + 1];

    res[0] = getPackageFlag(this->flag);
    for (int i = 0; i < len; i++)
        res[i + 1] = *(this->data + begin + i);
    if (index == p_amount){
        //last package
        return {res,this->last_length+1};
    };

    return {res,len+1};
}
int BigPackage::getAmountPackageToSend()
{
    return this->p_amount + 1;
}
