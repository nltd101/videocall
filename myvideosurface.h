#ifndef MYVIDEOSURFACE_H
#define MYVIDEOSURFACE_H
#ifndef WIDTH
#define WIDTH 360
#endif
#ifndef HEIGHT
#define HEIGHT 1000
#endif

#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QPainter>
#include <QCamera>
#include <QCameraInfo>
#include <QAbstractVideoSurface>
#include <QDateTime>
#include <QMutex>
#include <QList>
#include <vector>
#include <QObject>
#include <QBuffer>
#include <QMainWindow>
#include "udpservice.h"
#include "debugmarco.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class MyVideoSurface : public QAbstractVideoSurface
{

	// Q_OBJECT
private:
	QCamera *camera;
	UdpService *service;
	void (*onNewFrame)(void *, QImage);
	QMainWindow *context;
	void convertRGBImageToYUVchar(QImage *, char *);
	void convertYUVcharToRGBImage(char *, QImage *);
	void *contextNewFrame;

public:
	void setOnMyFrameListener(void *, void (*)(void *, QImage));
	MyVideoSurface(QObject *parent, QCamera *camera, UdpService *service);
	virtual ~MyVideoSurface();

	QList<QVideoFrame::PixelFormat>
	supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const;

	bool present(const QVideoFrame &frame);
};
#endif // MYVIDEOSURFACE_H
