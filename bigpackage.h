#ifndef BIGPACKAGE_H
#define BIGPACKAGE_H

#ifndef BUFLEN
#define BUFLEN 64000
#endif

#include <cstring>
#include <iostream>
#include <tuple>
class tuple;
class BigPackage
{
public:
    static const int FRAME = 1;
    static const int SOUND = 2;
    static const int FRAME_PACKAGE = 3;
    static const int SOUND_PACKAGE = 4;
    BigPackage(int amount, int flag);
    BigPackage(char *data, int length, int tag);
    BigPackage();
    void pushPackage(char *buf, int length);
    std::tuple<char *, int> getPackage(int index);
    static char getFlag(char *amount);
    static int getPackageFlag(int flag);
    std::tuple<char *, int> getCharData();
    int getFullLength();
    int getAmountPackageToSend();
    ~BigPackage();

private:
    int last_length;
    bool is_inited;
    int p_amount;
    char *data;
    int full_length;
    int flag;
};

#endif // BIGPACKAGE_H
