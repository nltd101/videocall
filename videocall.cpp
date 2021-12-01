#include "videocall.h"



void VideoCall::onRetriveFrame(void* parrent, char* data, int length){
   VideoCall*  displayer = static_cast<VideoCall *> (parrent);
   cout<<length<<endl;
   QImage image = QImage((uchar *)data, WIDTH, HEIGHT, QImage::Format_RGB32);
   displayer->handleNewVideoFrame(image);

}
void VideoCall::handleNewVideoFrame(QImage image){
    this->onPartnerFrameListener(context, image);
}
VideoCall::~VideoCall(){
    camera->stop();
    delete camera;
    delete udp_service;
}

VideoCall::VideoCall(QMainWindow * context, UdpService* udp_service,void (*onMyFrameListener)(QMainWindow*, QImage), void(*onPartnerFrameListener)(QMainWindow*,QImage))
{
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (cameras.size() > 0)
    {
        for (int i = 0; i < cameras.count(); ++i)
        {
            QCameraInfo cameraInfo = cameras.at(i);
            qDebug() << cameraInfo.deviceName();
            qDebug() << cameraInfo.description();
            camera = new QCamera(cameraInfo);
            break;
        }
    }
    else
    {
        cout<<"fail";
    }
    MyVideoSurface* surface = new MyVideoSurface(context, camera, udp_service,onMyFrameListener);
    camera->setViewfinder(surface);
    camera->start();
    this->udp_service=udp_service;
    this->onPartnerFrameListener = onPartnerFrameListener;
    this->context = context;
    this->udp_service->setReceiveFrameListener(this,&VideoCall::onRetriveFrame);
}

