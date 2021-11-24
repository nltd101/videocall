#ifndef UTILS_H
#define UTILS_H
#include <QImage>
#include <QBuffer>
class utils
{
public:
    utils();
    unsigned char* deepCopyImageData(int &size, QImage image);
};

#endif // UTILS_H
