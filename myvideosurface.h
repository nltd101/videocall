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
#include "udpservice.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

// https://www.qtcentre.org/threads/57090-How-could-I-get-the-image-buffer-of-QCamera
class MyVideoSurface : public QAbstractVideoSurface
{

//Q_OBJECT
private:
    Ui::MainWindow *ui;
    QCamera *camera;
    UdpService* service;


public:

    MyVideoSurface(QObject *parent, Ui::MainWindow *ui, QCamera *camera, UdpService* service);
    virtual ~MyVideoSurface();

    QList<QVideoFrame::PixelFormat>
    supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const;

    bool present(const QVideoFrame &frame);

private slots:
    void on_stopCallButton_clicked();
};
#endif // MYVIDEOSURFACE_H
