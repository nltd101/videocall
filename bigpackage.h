

#ifndef BIGPACKAGE_H
#define BIGPACKAGE_H

#ifndef BUFLEN
#define BUFLEN 64000
#endif

// #ifndef IMAGETYPE
// #define IMAGETYPE 1
// #endif
// #ifndef IMAGE_P
// #define IMAGE_P 2
// #endif
// #ifndef SOUND
// #define SOUND 3
// #endif
// #ifndef SOUND_P
// #define SOUND_P 4
// #endif

#include <cstring>
#include <iostream>

class BigPackage
{
public:
    static const int FRAME = 1;
    static const int SOUND = 2;
    static const int FRAME_PACKAGE = 3;
    static const int SOUND_PACKAGE = 4;
    BigPackage(char* amount, int length, int flag);
    BigPackage(char* data, int tag);
    BigPackage();
    void pushPackage(char* buf, int length);
    char* getPackage(int index);
    static char getFlag(char* amount);
    static int getPackageFlag(int flag);
    char* getCharData();
    int getAmountPackageToSend();
    ~BigPackage();
private:
    bool is_inited;
    int p_amount;
    char* data;
    int full_length;
    int flag;

};

#endif // BIGPACKAGE_H
