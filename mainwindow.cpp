#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::_onMyNewFrame(QMainWindow *mainWindow, QImage image)
{
	QPixmap pm = QPixmap::fromImage(image);
	MainWindow *context = (MainWindow *)mainWindow;
	context->getUi()->label->setPixmap(pm.scaled(context->getUi()->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void MainWindow::_onPartnerNewFrame(QMainWindow *mainWindow, QImage image)
{
	QPixmap pm = QPixmap::fromImage(image);
	MainWindow *context = (MainWindow *)mainWindow;
	context->getUi()->partner_label->setPixmap(pm.scaled(context->getUi()->partner_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
										  _ui(new Ui::MainWindow)
{
	_ui->setupUi(this);
	setAcceptDrops(true);

	_udpService = new UdpService();
	_audioCall = new AudioCall(this, _udpService);
	_videoCall = new VideoCall(this, _udpService, _onMyNewFrame, _onPartnerNewFrame);
}
Ui::MainWindow *MainWindow::getUi()
{
	return this->_ui;
}
MainWindow::~MainWindow()
{
	delete _videoCall;
	delete _udpService;
	delete _ui;
}

void MainWindow::on_startCallButton_clicked()
{
	QString port = _ui->portEditText->toPlainText();

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
		_udpService->setPartnerAddress("127.0.0.1", n);
	}

	_udpService->start();

	int myPort = _udpService->getMyPort();
	_ui->label_port->setText(QString("Your port is %1").arg(myPort));
	_ui->startCallButton->setEnabled(false);
	_ui->stopCallButton->setEnabled(true);
}
void MainWindow::on_stopCallButton_clicked()
{
	_ui->label_port->setText(QString("Your port is 0"));
	_ui->startCallButton->setEnabled(true);
	_ui->stopCallButton->setEnabled(false);
	_udpService->stop();
}
