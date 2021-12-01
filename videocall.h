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

class VideoCall
{
private:
    QCamera *camera;
    UdpService *udp_service;
    QMainWindow *context;
    void (*onPartnerFrameListener)(QMainWindow *, QImage);
    static void onRetriveFrame(void *, char *, int);

public:
    ~VideoCall();
    void handleNewVideoFrame(QImage);
    VideoCall(QMainWindow *context, UdpService *udp_service, void (*)(QMainWindow *, QImage), void (*)(QMainWindow *, QImage));
};

#endif // VIDEOCALL_H
