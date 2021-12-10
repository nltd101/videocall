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
private:
    QCamera *camera;
    UdpService *udpService;
    QMainWindow *context;
    void (*onPartnerFrameListener)(QMainWindow *, QImage);
    static void onRetriveFrame(void *, unsigned char *, int);
    static void onNewFrame(void *, QImage);
    std::tuple<unsigned char *, int> convertRGBImageToYUVchar(QImage);
    QImage convertYUVcharToRGBImage(unsigned char*,int);
    void (*onMyFrameListener)(QMainWindow *, QImage);
public:
    ~VideoCall();
    void handleReceivedFrame(unsigned char*,int);
    void handleNewFrame(QImage);
    VideoCall(QMainWindow *, UdpService *, void (*)(QMainWindow *, QImage), void (*)(QMainWindow *, QImage));
};

#endif // VIDEOCALL_H
