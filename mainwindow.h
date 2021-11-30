#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#ifndef WIDTH
#define WIDTH 300

#endif
#ifndef HEIGHT
#define HEIGHT 200

#endif
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
#include <QIODevice>
#include <QAudioOutput>
#include <QtMath>
#include <QAudioInput>
using namespace std;
namespace Ui
{
  class MainWindow;
}
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_startCallButton_clicked();
  void on_stopCallButton_clicked();
private slots:
  void handleStateChanged(QAudio::State newState);
  void stopRecording();

private:
  void startAudioCall();
  Ui::MainWindow *ui;
  QCamera *camera;
  QAudioInput *inputaudio;
  QAudioOutput *outputaudio;
  MyVideoSurface *surface;
  UdpService *udpservice;
  void sendData(char *buf);
};

#endif // MAINWINDOW_H
