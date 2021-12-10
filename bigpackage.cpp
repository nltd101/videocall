#include <string>
#include <cstring>
#include "bigpackage.h"
#include "iostream"

using namespace std;
BigPackage::BigPackage()
{
}
std::tuple<unsigned char*, int> BigPackage::getCharData()
{
    return { this->data, this->dataLength };
}
BigPackage::~BigPackage()
{
    if (this->isAllocatedData)
        delete this->data;
}

BigPackage::BigPackage(unsigned char* firstPackage, int length)
{
    int packageNums = (firstPackage[0] & 0x3f) | (firstPackage[1]);
    this->packageNums = packageNums;
    this->data = new unsigned char[packageNums * PACKAGESIZE];
    for (int i = 2; i < length; i++) {
        this->data[i - 2] = firstPackage[i];
    }
    this->isAllocatedData = true;
    this->flag = BigPackage::getFlag(firstPackage);
    this->dataLength = length - 2;
}

int BigPackage::getFullLength()
{
    return this->dataLength;
}

unsigned char BigPackage::getFlag(unsigned char* amount)
{

    return amount[0] >> 6;
}
BigPackage::BigPackage(unsigned char* data, int length, int flag)
{

    this->flag = flag;
    this->data = data;
    this->dataLength = length;
    this->packageNums = length / PACKAGESIZE + (length % PACKAGESIZE == 0 ? 0 : 1);
    this->isAllocatedData = false;
    this->lastLength = length % PACKAGESIZE == 0 ? PACKAGESIZE : length % PACKAGESIZE;
}

void BigPackage::pushPackage(unsigned char* buf, int length)
{
    if (BigPackage::getFlag(buf) != BigPackage::getPackageFlag(this->flag))
        return;
    unsigned short index = (buf[0] & 0x3f) | (buf[1]);
    for (int i = 2; i < length; i++)
    {
        this->data[index * PACKAGESIZE + i - 2] = buf[i];
    }
    if (index == this->packageNums - 1)
        this->lastLength = length - 2;

    dataLength += length - 2;
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
std::tuple<unsigned char*, int> BigPackage::getPackageToSend(int index)
{
    if (index != 0)
        index = packageNums - index;
    unsigned char byte1, byte2;
    int packageLength;
    if (index * PACKAGESIZE >= this->dataLength)
    {
        return { nullptr, 0 };
    }
    if (index == 0)
    {
        byte1 = (this->flag << 6) | (packageNums & 0x3f >> 8);
        byte2 = (packageNums & 0x00ff);
        if (packageNums == 1) {
            packageLength = this->lastLength + 2;

        }
        else packageLength = BUFLEN;
    }
    else
    {
        int pFlag = getPackageFlag(this->flag);
        byte1 = (pFlag << 6) | (packageNums & 0x3f >> 8);
        byte2 = (index & 0x00ff);
        if (index == packageNums - 1) {
            packageLength = this->lastLength + 2;
        }
        else {
            packageLength = BUFLEN;
        }
    }
    unsigned char* res = new unsigned char[packageLength];
    res[0] = byte1;
    res[1] = byte2;
    for (int i = 2; i < packageLength; i++)
        res[i] = *(this->data + index * PACKAGESIZE + i - 2);
    return { res, packageLength };

}
int BigPackage::getAmountPackageToSend()
{
    return this->packageNums;
}
