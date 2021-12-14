#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <iostream>
#include <QBuffer>
#include <QTimer>
#include <QMediaPlayer>
#include "myvideosurface.h"
#include "udpservice.h"
#include <QMessageBox>
#include <QByteArray>
#include <mutex>
#include <QProcess>
#include "audiocall.h"
#include "videocall.h"
#include "debugmarco.h"
using namespace std;

namespace Ui
{
	class MainWindow;
}
class VideoCall;
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	Ui::MainWindow *getUi();

private slots:
	void on_startCallButton_clicked();
	void on_stopCallButton_clicked();

private:
    static void _onMyNewFrame(QMainWindow *, QImage);
    static void _onPartnerNewFrame(QMainWindow *, QImage);
	void _startAudioCall();

	Ui::MainWindow *_ui;
	VideoCall *_videoCall;
	AudioCall *_audioCall;
	UdpService *_udpService;
};
#endif // MAINWINDOW_H
