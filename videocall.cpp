#include "videocall.h"

void VideoCall::_onRetriveFrame(void *parrent, unsigned char *data, int length)
{
	VideoCall *displayer = static_cast<VideoCall *>(parrent);

	displayer->handleReceivedFrame(data, length);
}
void VideoCall::_onNewFrame(void *parrent, QImage image)
{
	VideoCall *displayer = static_cast<VideoCall *>(parrent);
	displayer->handleNewFrame(image);
}
void VideoCall::handleReceivedFrame(unsigned char *data, int length)
{
	QImage image = _convertYUVcharToRGBImage(data, length);
	_onPartnerFrameListener(_context, image);
}
void VideoCall::handleNewFrame(QImage image)
{
	_onMyFrameListener(_context, image);
	image = image.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);
	unsigned char *data;
	int length;
	tie(data, length) = _convertRGBImageToYUVchar(image);

	_udpService->sendFrame(data, length);
	delete data;
}
void convertYUVtoRGB(unsigned char Y, unsigned char U, unsigned char V, unsigned char &R, unsigned char &G, unsigned char &B)
{

	R = (max(0, 256 * Y - 24 * U + 24 * 128 + 372 * V - 372 * 128) >> 8);
	G = (max(0, 256 * Y - 174 * U + 174 * 128 - 194 * V + 193 * 128) >> 8);
	B = (max(0, 256 * Y + 392 * U - 392 * 128 + 9 * V - 9 * 128) >> 8);
}
void convertRGBtoYUV(unsigned char R, unsigned char G, unsigned char B, unsigned char &Y, unsigned char &U, unsigned char &V)
{
	Y = ((66 * R + 129 * G + 61 * B) >> 8);

	U = ((-46 * R - 82 * G + 128 * B) >> 8) + 128;

	V = ((128 * R - 94 * G - 34 * B) >> 8) + 128;
}
std::tuple<unsigned char *, int> VideoCall::_convertRGBImageToYUVchar(QImage image)
{
	unsigned short width = image.width();
	unsigned short height = image.height();
	int yLen = width * height;
	int uvLen = ((width + 1) / 2) * ((height + 1) / 2);

	unsigned char *data = new unsigned char[4 + yLen + 2 * uvLen];
	data[0] = width >> 8;
	data[1] = width & 0xFF;
	data[2] = height >> 8;
	data[3] = height & 0xFF;

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			QColor clrCurrent(image.pixel(x, y));
			unsigned char red = clrCurrent.red();
			unsigned char green = clrCurrent.green();
			unsigned char blue = clrCurrent.blue();
			unsigned char Y, U, V;
			convertRGBtoYUV(red, green, blue, Y, U, V);
			data[4 + y * width + x] = (char)Y;
			if (x % 2 == 0 && y % 2 == 0)
			{
				data[4 + yLen + y / 2 * ((width + 1) / 2) + x / 2] = (char)U;
				data[4 + yLen + uvLen + y / 2 * ((width + 1) / 2) + x / 2] = (char)V;
			}
		}

	return {data, 4 + yLen + 2 * uvLen};
}
QImage VideoCall::_convertYUVcharToRGBImage(unsigned char *data, int length)
{
	unsigned short width = ((unsigned char)data[0] << 8) | (unsigned char)data[1];
	unsigned short height = ((unsigned char)data[2] << 8) | (unsigned char)data[3];

	int yLen = width * height;
	int uvLen = ((width + 1) / 2) * ((height + 1) / 2);
	char *imgbuf = new char[width * height * 4];
	QImage image(width, height, QImage::Format_RGB32);
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			unsigned char Y = (unsigned char)data[4 + y * width + x];
			unsigned char U = (unsigned char)data[4 + yLen + y / 2 * ((width + 1) / 2) + x / 2];
			unsigned char V = (unsigned char)data[4 + yLen + uvLen + y / 2 * ((width + 1) / 2) + x / 2];
			unsigned char R, G, B;
			convertYUVtoRGB(Y, U, V, R, G, B);

			QColor clrCurrent = QColor((unsigned char)R, (unsigned char)G, (unsigned char)B);

			image.setPixelColor(x, y, clrCurrent);
		}
	delete imgbuf;
	return image;
}
VideoCall::~VideoCall()
{
	_camera->stop();
	delete _camera;
	delete _udpService;
}
VideoCall::VideoCall(QMainWindow *context, UdpService *udpService, void (*onMyFrameListener)(QMainWindow *, QImage), void (*onPartnerFrameListener)(QMainWindow *, QImage))
{
	QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
	if (cameras.size() > 0)
	{
		for (int i = 0; i < cameras.count(); ++i)
		{
			QCameraInfo cameraInfo = cameras.at(i);
			qDebug() << cameraInfo.deviceName();
			qDebug() << cameraInfo.description();
			_camera = new QCamera(cameraInfo);
			break;
		}
	}
	else
	{
		cout << "fail";
	}
	MyVideoSurface *surface = new MyVideoSurface(context, _camera, udpService);
	surface->setOnMyFrameListener(this, &VideoCall::_onNewFrame);
	_camera->setViewfinder(surface);
	_camera->start();
	_udpService = udpService;
	_onPartnerFrameListener = onPartnerFrameListener;
	_context = context;
	_udpService->setReceiveFrameListener(this, &VideoCall::_onRetriveFrame);
	_onMyFrameListener = onMyFrameListener;
}
