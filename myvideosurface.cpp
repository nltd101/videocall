#include "myvideosurface.h"
#include "./ui_mainwindow.h"

MyVideoSurface::MyVideoSurface(QObject *parent, Ui::MainWindow *ui, QCamera *camera) : QAbstractVideoSurface(parent)
{

    this->ui = ui;
    this->camera = camera;
}

MyVideoSurface::~MyVideoSurface()
{
}

QList<QVideoFrame::PixelFormat> MyVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
    if (type == QAbstractVideoBuffer::NoHandle)
    {
        return QList<QVideoFrame::PixelFormat>()
               << QVideoFrame::Format_RGB32;
    }
    else
    {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool MyVideoSurface::present(const QVideoFrame &frame)
{
    if (frame.isValid())
    {
        this->count++;
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        const QImage img(cloneFrame.bits(),
                         cloneFrame.width(),
                         cloneFrame.height(),
                         QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));

        QImage cp = img.copy();

        cloneFrame.unmap();

        cp = cp.mirrored(true, false);

        QPixmap pm = QPixmap::fromImage(cp);
        ui->label->setPixmap(pm.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        return true;
    }
    return false;
}

