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
    UdpService *udp_service;
    QMainWindow *context;
    void (*onPartnerFrameListener)(QMainWindow *, QImage);
    static void onRetriveFrame(void *, char *, int);
    static void onNewFrame(void *, QImage);
    char* convertRGBImageToYUVchar(QImage);
    QImage convertYUVcharToRGBImage(char*);
    void (*onMyFrameListener)(QMainWindow *, QImage);
public:
    ~VideoCall();
    void handleReceivedFrame(QImage);
    void handleNewFrame(QImage);
    VideoCall(QMainWindow *context, UdpService *udp_service, void (*)(QMainWindow *, QImage), void (*)(QMainWindow *, QImage));
};

#endif // VIDEOCALL_H
