#include "mainwindow.h"
#include "ui_mainwindow.h"




void MainWindow::onMyNewFrame(QMainWindow *main_window, QImage image)
{
    QPixmap pm = QPixmap::fromImage(image);
    MainWindow *context = (MainWindow *)main_window;
    context->getUi()->label->setPixmap(pm.scaled(context->getUi()->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void MainWindow::onPartnerNewFrame(QMainWindow *main_window, QImage image)
{
    QPixmap pm = QPixmap::fromImage(image);
    MainWindow *context = (MainWindow *)main_window;
    context->getUi()->partner_label->setPixmap(pm.scaled(context->getUi()->partner_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    udpservice = new UdpService();
    audiocall = new AudioCall(this,udpservice);
    videocall = new VideoCall(this,udpservice,onMyNewFrame,onPartnerNewFrame);
}

MainWindow::~MainWindow()
{
    delete videocall;
    delete udpservice;
    delete ui;
}

void MainWindow::on_startCallButton_clicked()
{
    QString port = ui->portEditText->toPlainText();

    if (port.length() == 0)
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Please input the valid port");
        messageBox.setFixedSize(500, 200);
        return;
    }
    int n = port.toInt();

    if (n != 0)
    {
        udpservice->setPartnerAddress("127.0.0.1", n);
    }

    udpservice->start();

    int myport = udpservice->getMyPort();
    ui->label_port->setText(QString("Your port is %1").arg(myport));
    ui->startCallButton->setEnabled(false);
    ui->stopCallButton->setEnabled(true);
}
void MainWindow::on_stopCallButton_clicked()
{
    ui->label_port->setText(QString("Your port is 0"));
    ui->startCallButton->setEnabled(true);
    ui->stopCallButton->setEnabled(false);
    this->udpservice->stop();
}
