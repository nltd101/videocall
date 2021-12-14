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
public:
    AudioCall(QMainWindow *, UdpService *);
    ~AudioCall();

    void sound(unsigned char *, int);

private:
    UdpService *_udpService;
    QMainWindow *_context;
    QIODevice *_inputDevice;
    QIODevice *_outputDevice;
    QAudioInput *_inputAudio;
    QAudioOutput *_outputAudio;
private slots:
    void handleStateChanged(QAudio::State);
    static void onRetriveSound(void *, unsigned char *, int);
};

#endif // AUDIOCALL_H
