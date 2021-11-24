#include "mainwindow.h"
#include "ui_mainwindow.h"

void updateLabel(Ui::MainWindow *ui,char* data){

//   QByteArray arr =  QByteArray::fromRawData(data,strlen(data));
//            QPixmap pix;
//    pix.loadFromData(arr,"JPG");
    QImage image = QImage((uchar*)data,400,500,QImage::Format_RGB888);
//    ui->partner_label->setPixmap(pix);
   // ui->partner_label->setText(QString("%1").arg(strlen(data)));
  QPixmap pm = QPixmap::fromImage(image);
    ui->partner_label->setPixmap(pm.scaled(ui->partner_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

//    cout<<"length"<<strlen(data)<<endl;

}
using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
        ui->setupUi(this);
        setAcceptDrops(true);
          udpservice = new UdpService(ui);
        camera = NULL;
        QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        if (cameras.size() > 0)
        {
            for( int i = 0; i < cameras.count(); ++i )
            {
                QCameraInfo cameraInfo = cameras.at(i);
                qDebug() << cameraInfo.deviceName();
                qDebug() << cameraInfo.description();
                camera = new QCamera(cameraInfo);
                surface = new MyVideoSurface(this, ui, camera,udpservice);
                camera->setViewfinder(surface);
                camera->start();
                break;
            }
        }
        else {
            ui->label->setText("No camera");
        }

        udpservice->onReceiver=updateLabel;
}
void MainWindow::sendData(char* buf){
    this->udpservice->send(buf);
}


MainWindow::~MainWindow()
{
    camera->stop();
    delete camera;
    delete surface;
    delete ui;
}

void MainWindow::on_startCallButton_clicked()
{
    QString port = ui->portEditText->toPlainText();

     if ( port.length()==0){
         QMessageBox messageBox;
         messageBox.critical(0,"Error","Please input the valid port");
         messageBox.setFixedSize(500,200);
         return;
     }
     int n = port.toInt();

     if (n!=0){
           udpservice->setPartnerAddress("0.0.0.0",n);
     }

    udpservice->start();

    int myport=udpservice->getMyPort();
    ui->startCallButton->setText(QString("Your port is %1").arg(myport));
    ui->startCallButton->setEnabled(false);
}
