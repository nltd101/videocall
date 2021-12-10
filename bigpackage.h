#ifndef BIGPACKAGE_H
#define BIGPACKAGE_H

#ifndef BUFLEN
#define BUFLEN 1460
const int PACKAGESIZE = BUFLEN - 2;
#endif
#include <cstring>
#include <iostream>
#include <tuple>
#include <bitset>
class tuple;
class BigPackage
{
public:
    static const int FRAME = 0;
    static const int SOUND = 2;
    static const int FRAME_PACKAGE = 1;
    static const int SOUND_PACKAGE = 3;

    BigPackage(unsigned char* firstPackage, int length);
    BigPackage(unsigned char* data, int length, int tag);
    BigPackage();
    void pushPackage(unsigned char* buf, int length);
    std::tuple<unsigned char*, int> getPackageToSend(int index);
    static unsigned char getFlag(unsigned char* amount);
    static int getPackageFlag(int flag);
    std::tuple< unsigned char*, int> getCharData();
    int getFullLength();
    int getAmountPackageToSend();
    ~BigPackage();

private:


    int lastLength;
    bool isAllocatedData;
    int packageNums;
    unsigned char* data;
    int dataLength;
    int flag;
};

#endif // BIGPACKAGE_H
