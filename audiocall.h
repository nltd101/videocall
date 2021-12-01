#ifndef AUDIOCALL_H
#define AUDIOCALL_H

#include <QIODevice>
#include <QAudioOutput>
#include <QtMath>
#include <QAudioInput>
#include <QAudioFormat>
#include "udpservice.h"
#include <QMainWindow>
#include <QIODevice>
#include <QDebug>
#include <QBuffer>
class AudioCall
{
private:
  UdpService *udp_service;
  QMainWindow *context;
  QIODevice *input_device;
  QIODevice *output_device;
  QAudioInput *inputaudio;
  QAudioOutput *outputaudio;
private slots:
  void handleStateChanged(QAudio::State newState);
static void onRetriveSound(void* parrent, char* data, int length);
public:

  void sound(char *data, int length);
  AudioCall(QMainWindow *, UdpService *);

};

#endif // AUDIOCALL_H
