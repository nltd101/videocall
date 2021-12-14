#include <string>
#include <cstring>
#include "bigpackage.h"
#include "iostream"

using namespace std;
BigPackage::BigPackage()
{
}
std::tuple<unsigned char *, int> BigPackage::getCharData()
{
	return {_data, _dataLength};
}
BigPackage::~BigPackage()
{
	if (_isAllocatedData)
		delete _data;
}

BigPackage::BigPackage(unsigned char *firstPackage, int length)
{
	int packageNums = (firstPackage[0] & 0x3f) | (firstPackage[1]);
	this->_packageNums = packageNums;
	_data = new unsigned char[packageNums * PACKAGESIZE];
	for (int i = 2; i < length; i++)
	{
		_data[i - 2] = firstPackage[i];
	}
	_isAllocatedData = true;
	_flag = BigPackage::getFlag(firstPackage);
	_dataLength = length - 2;
}

int BigPackage::getFullLength()
{
	return _dataLength;
}

unsigned char BigPackage::getFlag(unsigned char *amount)
{

	return amount[0] >> 6;
}
BigPackage::BigPackage(unsigned char *data, int length, int flag)
{

	_flag = flag;
	_data = data;
	_dataLength = length;
	_packageNums = length / PACKAGESIZE + (length % PACKAGESIZE == 0 ? 0 : 1);
	_isAllocatedData = false;
	_lastLength = length % PACKAGESIZE == 0 ? PACKAGESIZE : length % PACKAGESIZE;
}

void BigPackage::pushPackage(unsigned char *buf, int length)
{
	if (BigPackage::getFlag(buf) != BigPackage::getPackageFlag(_flag))
		return;
	unsigned short index = (buf[0] & 0x3f) | (buf[1]);
	for (int i = 2; i < length; i++)
	{
		_data[index * PACKAGESIZE + i - 2] = buf[i];
	}
	if (index == _packageNums - 1)
		_lastLength = length - 2;

	_dataLength += length - 2;
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
std::tuple<unsigned char *, int> BigPackage::getPackageToSend(int index)
{
	if (index != 0)
		index = _packageNums - index;
	unsigned char byte1, byte2;
	int packageLength;
	if (index * PACKAGESIZE >= _dataLength)
	{
		return {nullptr, 0};
	}
	if (index == 0)
	{
		byte1 = (_flag << 6) | (_packageNums & 0x3f >> 8);
		byte2 = (_packageNums & 0x00ff);
		if (_packageNums == 1)
		{
			packageLength = _lastLength + 2;
		}
		else
			packageLength = BUFLEN;
	}
	else
	{
		int pFlag = getPackageFlag(_flag);
		byte1 = (pFlag << 6) | (_packageNums & 0x3f >> 8);
		byte2 = (index & 0x00ff);
		if (index == _packageNums - 1)
		{
			packageLength = _lastLength + 2;
		}
		else
		{
			packageLength = BUFLEN;
		}
	}
	unsigned char *res = new unsigned char[packageLength];
	res[0] = byte1;
	res[1] = byte2;
	for (int i = 2; i < packageLength; i++)
		res[i] = *(_data + index * PACKAGESIZE + i - 2);
	return {res, packageLength};
}
int BigPackage::getAmountPackageToSend()
{
	return _packageNums;
}
