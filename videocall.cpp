#include "videocall.h"

void VideoCall::onRetriveFrame(void *parrent, char *data, int length)
{
    VideoCall *displayer = static_cast<VideoCall *>(parrent);
    cout <<"length: receive : "<< length << endl;
    data[length]='\0';
    QImage image =  QImage((uchar *)data, WIDTH, HEIGHT, QImage::Format_RGB32);
    displayer->handleReceivedFrame(image);

}
void VideoCall::onNewFrame(void *parrent, QImage image)
{
    VideoCall *displayer = static_cast<VideoCall *>(parrent);
    //cout <<"length: receive : "<< length << endl;
//    data[length]='\0';
    //QImage image = QImage((uchar *)data, WIDTH, HEIGHT, QImage::Format_RGB888);
    displayer->handleNewFrame(image);
}
void VideoCall::handleReceivedFrame(QImage image)
{
    this->onPartnerFrameListener(context, image);
}
void VideoCall::handleNewFrame(QImage image)
{
    this->onMyFrameListener(context,image);
    image = image.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);
    char *data = this->convertRGBImageToYUVchar(image);

    QImage image2 = this->convertYUVcharToRGBImage(data);
    //delete data;
    this->onPartnerFrameListener(context,image2);
}
void convertYUVtoRGB(unsigned char Y, unsigned char U, unsigned char V, unsigned char& R, unsigned char& G, unsigned char& B) {

    R = ((256 * Y - 24 * U + 24 * 128 + 372 * V - 372 * 128) >> 8);
    G = ((256 * Y - 173 * U + 173 * 128 - 193 * V + 193 * 128) >> 8);
    B = ((256 * Y + 392 * U - 392 * 128 + 8 * V - 8 * 128) >> 8);


}
void convertRGBtoYUV(unsigned char R, unsigned char G, unsigned char B, unsigned char& Y, unsigned char& U, unsigned char& V) {
    //    [0.257, 0.504, 0.239], [-0.178, - 0.322, 0.500],
    //                  [0.500, - 0.368, - 0.132, ]
    Y = ((66 * R + 129 * G + 61 * B) >> 8);

    U = ((-46 * R - 82 * G + 128 * B) >> 8) + 128;

    V = ((128 * R - 94 * G - 34 * B) >> 8) + 128;
}

char* VideoCall::convertRGBImageToYUVchar(QImage image){
//    [0.257, 0.504, 0.239], [-0.178, - 0.322, 0.500],
//                  [0.500, - 0.368, - 0.132, ]
//    for ()
    unsigned short width = image.width();
    unsigned short height = image.height();
    int y_len = width*height;
    int uv_len = ((width+1)/2)*((height+1)/2);

    char* data= new char[4+y_len+2*uv_len];
    data[0] = width >> 8;
    data[1] = width & 0xFF;
    data[2] = height >> 8;
    data[3] = height & 0xFF;

    int maxlength=0;
    for (int y = 0; y< height; y++)
        for (int x = 0; x< width; x++){
            QColor clrCurrent( image.pixel(x,y));
            unsigned char red = clrCurrent.red();
            unsigned char green = clrCurrent.green();
            unsigned char blue = clrCurrent.blue();
            unsigned char Y, U,V;
            convertRGBtoYUV(red,green,blue,Y,U,V);
            data[4+y*width+x]=(char)Y;
            if (x%2==0 && y%2==0)
            {
                data[4+y_len+ y/2*((width+1)/2)+x/2]=(char)U;
                data[4+y_len+uv_len+y/2*((width+1)/2)+x/2]=(char)V;
                maxlength=4+y_len+uv_len+(y/2)*((width+1)/2)+x/2;
            }
    }
    cout<<maxlength<<endl;
    return data;
}
QImage VideoCall::convertYUVcharToRGBImage(char* data){
    unsigned short width = ((unsigned char)data[0]<<8) | (unsigned char)data[1];
    unsigned short height = ((unsigned char)data[2]<<8)|(unsigned char)data[3];

    int y_len = width*height;
    int uv_len = ((width+1)/2)*((height+1)/2);
    char* imgbuf = new char[width*height*4];
    QImage image(width,height, QImage::Format_RGB32);
    for (int y = 0; y< height; y++)
    for (int x = 0; x< width; x++){
        unsigned char Y = (unsigned char) data[4+y*width+x];
        unsigned char U = (unsigned char)data[4+y_len+ y/2*((width+1)/2)+x/2];
        unsigned char V = (unsigned char) data[4+y_len+uv_len+y/2*((width+1)/2)+x/2];
        unsigned char R,G,B;
        convertYUVtoRGB(Y,U,V,R,G,B);

        QColor clrCurrent= QColor((unsigned char)R,(unsigned char)G,(unsigned char)B);

        image.setPixelColor(x,y,clrCurrent);
        if (x==5 && y==5){
            mlog("stop");
        }
    }
    delete imgbuf;
    return image;
}
VideoCall::~VideoCall()
{
    camera->stop();
    delete camera;
    delete udp_service;
}
VideoCall::VideoCall(QMainWindow *context, UdpService *udp_service, void (*onMyFrameListener)(QMainWindow *, QImage), void (*onPartnerFrameListener)(QMainWindow *, QImage))
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
        cout << "fail";
    }
    MyVideoSurface *surface = new MyVideoSurface(context, camera, udp_service);
    surface->setOnMyFrameListener(this,&VideoCall::onNewFrame);
    camera->setViewfinder(surface);
    camera->start();
    this->udp_service = udp_service;
    this->onPartnerFrameListener = onPartnerFrameListener;
    this->context = context;
    this->udp_service->setReceiveFrameListener(this, &VideoCall::onRetriveFrame);
    this->onMyFrameListener = onMyFrameListener;
}
