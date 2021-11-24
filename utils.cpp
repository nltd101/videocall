#include "utils.h"

utils::utils()
{

}
unsigned char* utils::deepCopyImageData(int &size,QImage image){

  QByteArray bytes;
  QBuffer buffer(&bytes);
  buffer.open(QIODevice::WriteOnly);
  image.save(&buffer, "PNG");
  buffer.close();

  unsigned char *data = (unsigned char *) malloc(bytes.size());
  memcpy(data, reinterpret_cast<unsigned char *>(bytes.data()), bytes.size());
  size = bytes.size();
  return data;
}
