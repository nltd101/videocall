#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define DEBUG_MODE true
#define mdebug(x)                  \
  do                               \
  {                                \
    if (DEBUG_MODE)                \
    {                              \
      std::cerr << x << std::endl; \
    }                              \
  } while (0)
#define mlog(x)                    \
  do                               \
  {                                \
    if (DEBUG_MODE)                \
    {                              \
      std::cout << x << std::endl; \
    }                              \
  } while (0)

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
  Ui::MainWindow *getUi()
  {
    return this->ui;
  }

private slots:
  void on_startCallButton_clicked();
  void on_stopCallButton_clicked();

private:
  void startAudioCall();
  Ui::MainWindow *ui;
  VideoCall *videocall;
  AudioCall * audiocall;
  static void onMyNewFrame(QMainWindow *main_window, QImage image);
  static void onPartnerNewFrame(QMainWindow *main_window, QImage image);


  UdpService *udpservice;

};
#endif // MAINWINDOW_H
