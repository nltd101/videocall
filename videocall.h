#ifndef VIDEOCALL_H
#define VIDEOCALL_H
#include "mainwindow.h"
#include "udpservice.h"
#include <QCamera>
#include <QMainWindow>
#include <QCameraInfo>
#include "myvideosurface.h"
#include "mainwindow.h"
#include <QImage>
#include "debugmarco.h"
class VideoCall
{

public:
	~VideoCall();
	VideoCall(QMainWindow *, UdpService *, void (*)(QMainWindow *, QImage), void (*)(QMainWindow *, QImage));

	void handleReceivedFrame(unsigned char *, int);
	void handleNewFrame(QImage);

private:
	QCamera *_camera;
	UdpService *_udpService;
	QMainWindow *_context;
	void (*_onPartnerFrameListener)(QMainWindow *, QImage);
	static void _onRetriveFrame(void *, unsigned char *, int);
	static void _onNewFrame(void *, QImage);
	std::tuple<unsigned char *, int> _convertRGBImageToYUVchar(QImage);
	QImage _convertYUVcharToRGBImage(unsigned char *, int);
	void (*_onMyFrameListener)(QMainWindow *, QImage);
};

#endif // VIDEOCALL_H
