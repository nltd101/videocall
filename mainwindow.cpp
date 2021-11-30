#include "mainwindow.h"
#include "ui_mainwindow.h"

void updateLabel(Ui::MainWindow *ui, char *data)
{
    cout << "receive full: " << strlen(data) << endl;
    QImage image = QImage((uchar *)data, WIDTH, HEIGHT, QImage::Format_RGB32);

    //  QSize size= image.size();
    cout << "receive: " << strlen(data) << endl;
    QPixmap pm = QPixmap::fromImage(image);
    ui->partner_label->setPixmap(pm.scaled(ui->partner_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
void startVoice(Ui::MainWindow *ui, char *data)
{
    qreal sampleRate = 8000;   // sample rate
      qreal duration = 1.000;     // duration in seconds
      qreal frequency = 1000;     // frequency
      const quint32 n = 1000;//static_cast<quint32>(duration * sampleRate);   // number of data points

      QByteArray* byteBuffer = new QByteArray();
      byteBuffer->resize(n);

      for (int i = 0; i < n; i++) {
        qreal sinVal =  (qreal)qSin(2.0 * M_PI * frequency * i / sampleRate) ;
        sinVal *=32000;// MAX_VAL_16BIT;     // scale up y from +-1 to its full range
        (*byteBuffer)[i] = (qint16)sinVal;    // use qint16 (instead of quint16), because our waveform goes above and below zeros.
      }
        cout<<byteBuffer->size()<<endl;
      // create and setup a QAudioFormat object
      QAudioFormat format;
      format.setSampleRate(8000);
      format.setChannelCount(1);

      format.setSampleSize(8);
      format.setCodec("audio/pcm");
      format.setByteOrder(QAudioFormat::BigEndian);
      format.setSampleType(QAudioFormat::UnSignedInt);  // use SignedInt, because our waveform goes above and below zero.


      QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
      if(!deviceInfo.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
      }

      // Make a QBuffer with our QByteArray
      QBuffer* input = new QBuffer(byteBuffer);
      input->open(QIODevice::ReadOnly);   // set the QIODevice to read-only

      // Create an audio output with our QAudioFormat
      QAudioOutput* audio = new QAudioOutput(format);


      MainWindow::connect(audio, &QAudioOutput::stateChanged, [audio, input](QAudio::State newState) {
        if (newState == QAudio::IdleState ) { // Finished playing (no more data)
          qDebug() << "finish:";
          delete audio;
          delete input;
        }
        // could / should also handle more states. like error
      } );


      audio->start(input);

}
void MainWindow::stopRecording()
{
    this->inputaudio->stop();
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    setAcceptDrops(true);
    udpservice = new UdpService(ui);
    camera = NULL;
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (cameras.size() > 0)
    {
        for (int i = 0; i < cameras.count(); ++i)
        {
            QCameraInfo cameraInfo = cameras.at(i);
            qDebug() << cameraInfo.deviceName();
            qDebug() << cameraInfo.description();
            camera = new QCamera(cameraInfo);
            surface = new MyVideoSurface(this, ui, camera, udpservice);
                           camera->setViewfinder(surface);
                            camera->start();
            break;
        }
    }
    else
    {
        ui->label->setText("No camera");
    }
    udpservice->setReceiveFrameListener(
        updateLabel

    );
    udpservice->setReceiveSoundListener(startVoice);



}

void MainWindow::handleStateChanged(QAudio::State newState)
{
    switch (newState)
    {
    case QAudio::StoppedState:
        if (inputaudio->error() != QAudio::NoError)
        {
            // Error handling
        }
        else
        {
            // Finished recording
        }
        break;

    case QAudio::ActiveState:

        // Started recording - read from IO device
        break;

    default:
        // ... other cases as appropriate
        break;
    }
}

void MainWindow::startAudioCall()
{
    QAudioFormat format;
    // Set up the desired format, for example:
    format.setSampleRate(8000);
    format.setChannelCount(1);

    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::BigEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format))
    {
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
    }

    inputaudio = new QAudioInput(format, this);
    outputaudio = new QAudioOutput(format, this);
    connect(inputaudio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

    //   QTimer::singleShot(500, this, SLOT(stopRecording()));
    // audio->start();
    auto io = inputaudio->start();
    int count = 0;
    connect(io, &QIODevice::readyRead, [this, io, &count]
            {
                QByteArray buff = io->readAll();
                int buffLenght = buff.length();
                if (buffLenght > 0)
                {
                    char *data = (char *)buff.constData();
                    //     printf("%d\n",data[0]);
                    this->udpservice->sendSound(data);
                }
            });

}

MainWindow::~MainWindow()
{
    camera->stop();
    delete inputaudio;
    delete outputaudio;
    delete camera;
    delete surface;
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
