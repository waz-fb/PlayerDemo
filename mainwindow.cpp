#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMenuBar"
#include "decoder.h"
#include "resampler.h"
#include "audioplayer.h"
#include "audiothread.h"
#include "videothread.h"
#include "demuxthread.h"

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QThread>
#include <iostream>
using namespace std;


class TestThread :public QThread
{
private:
    AudioThread at;
    VideoThread vt;
public:
    void Init(const char* url)
    {
        demux = new Demux();
        cout << "demux.Open = " << demux->Open(url);
        cout << "CopyVPara = " << demux->getVideoParameter() << endl;
        cout << "CopyAPara = " << demux->getAudioParameter() << endl;

        cout << "vdecode.Open() = " << vdecode.Open(demux->getVideoParameter()) << endl;
//        cout << "adecode.Open() = " << adecode.Open(demux->getAudioParameter()) << endl;

//        cout << "resample.Open = " << resample.Open(demux->getAudioParameter()) << endl;
//        AudioPlayer::Get()->channels = demux->channels;
//        AudioPlayer::Get()->sampleRate = demux->sampleRate;

//       cout << "AudioPlayer::Get()->Open() = " << AudioPlayer::Get()->Open()<<endl;

        at.Open(demux->getAudioParameter(), demux->sampleRate, demux->channels);
        vt.Open(demux->getVideoParameter(), video, demux->width, demux->height);
        at.start();
        vt.start();
    }
    void run()
    {
        for (;;)
        {
            AVPacket *pkt = demux->Read();

            if (demux->IsAudio(pkt))
            {
                at.Push(pkt);
//                unsigned char *pcm = new unsigned char[1024 * 1024];
//                adecode.Send(pkt);
//                AVFrame *frame = adecode.Recv();
//                int len = resample.Resample(frame, pcm);
//                //cout<<"Resample:"<<len<<endl;
//                while (len > 0)
//                {
//                    if (AudioPlayer::Get()->isWritable(len))
//                    {
//                        AudioPlayer::Get()->Write(pcm, len);
//                        break;
//                    }
//                    msleep(1);
//                }
//                delete[] pcm;
            }
            else
            {
                vt.Push(pkt);
//                vdecode.Send(pkt);
//                AVFrame *frame = vdecode.Recv();
//                video->Repaint(frame);
                //msleep(40);
               // cout << "Video:" << frame << endl;
            }
            if (!pkt)break;
        }
    }
    Demux* demux;
    Decoder vdecode;
    Decoder adecode;
    VideoWidget *video;
    Resampler resample;

};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    createActions();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
    QFileDialog fileDialog(this, tr("Open File..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);

    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fn = fileDialog.selectedFiles().constFirst();

    // Live Stream source
    //rtmp://cbs-live.gscdn.com/cbs-live/cbs-live.stream'

//    testThread->video =  ui->videoWidget;
//    testThread->Init(fn.toLocal8Bit().data());
////    ui->videoWidget->Init(testThread->demux->width, testThread->demux->height);
//    testThread->start();
    //
    dt = new DemuxThread();
    dt->Open(fn.toLocal8Bit().data(), ui->videoWidget);
    dt->Start();
    cout<<"finished"<<endl;

}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(openAct);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::createActions() {
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
}

