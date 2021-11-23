#ifndef MYVIDEOSURFACE_H
#define MYVIDEOSURFACE_H

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


public:
     int count=0;
    MyVideoSurface(QObject *parent, Ui::MainWindow *ui, QCamera *camera);
    virtual ~MyVideoSurface();

    QList<QVideoFrame::PixelFormat>
    supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const;

    bool present(const QVideoFrame &frame);
};
#endif // MYVIDEOSURFACE_H
