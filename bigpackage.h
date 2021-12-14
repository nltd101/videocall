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
	BigPackage(unsigned char *, int);
	BigPackage(unsigned char *, int, int);
	BigPackage();
	~BigPackage();

	static const int FRAME = 0;
	static const int SOUND = 2;
	static const int FRAME_PACKAGE = 1;
	static const int SOUND_PACKAGE = 3;

	void pushPackage(unsigned char *, int);
	std::tuple<unsigned char *, int> getPackageToSend(int);
	static unsigned char getFlag(unsigned char *);
	static int getPackageFlag(int);
	std::tuple<unsigned char *, int> getCharData();
	int getFullLength();
	int getAmountPackageToSend();

private:
	int _lastLength;
	bool _isAllocatedData;
	int _packageNums;
	unsigned char *_data;
	int _dataLength;
	int _flag;
};

#endif // BIGPACKAGE_H
