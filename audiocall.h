#ifndef AUDIOCALL_H
#define AUDIOCALL_H

#include <QIODevice>
#include <QtMultimedia/QAudioOutput>
#include <QtMath>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioFormat>
#include "udpservice.h"
#include <QMainWindow>
#include <QIODevice>
#include <QDebug>
#include <QBuffer>
class AudioCall
{
private:
  UdpService *udpService;
  QMainWindow *context;
  QIODevice *inputDevice;
  QIODevice *outputDevice;
  QAudioInput *inputAudio;
  QAudioOutput *outputAudio;
private slots:
  void handleStateChanged(QAudio::State newState);
  static void onRetriveSound(void *parrent,unsigned char *data, int length);

public:
  ~AudioCall();
  void sound(unsigned char *data, int length);
  AudioCall(QMainWindow *, UdpService *);
};

#endif // AUDIOCALL_H
