#ifndef MYVIDEOSURFACE_H
#define MYVIDEOSURFACE_H
#ifndef WIDTH
#define WIDTH 300
#endif
#ifndef HEIGHT
#define HEIGHT 200
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
    void (*onNewFrame)(QMainWindow *, QImage);
    QMainWindow *context;

public:
    MyVideoSurface(QObject *parent, QCamera *camera, UdpService *service, void (*onNewFrame)(QMainWindow *, QImage));
    virtual ~MyVideoSurface();

    QList<QVideoFrame::PixelFormat>
    supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const;

    bool present(const QVideoFrame &frame);
};
#endif // MYVIDEOSURFACE_H
