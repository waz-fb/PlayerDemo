#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMenuBar"
#include "decoder.h"

#include <QContextMenuEvent>
#include <QFileDialog>


#include <iostream>
using namespace std;


class TestThread :public QThread
{
public:
    void Init(const char* url)
    {
        demux = new Demux();
        cout << "demux.Open = " << demux->Open(url);
        cout << "CopyVPara = " << demux->getVideoParameter() << endl;
        cout << "CopyAPara = " << demux->getAudioParameter() << endl;

        cout << "vdecode.Open() = " << vdecode.Open(demux->getVideoParameter()) << endl;
        cout << "adecode.Open() = " << adecode.Open(demux->getAudioParameter()) << endl;

    }
    void run()
    {
        for (;;)
        {
            AVPacket *pkt = demux->Read();
            if (demux->IsAudio(pkt))
            {
                //adecode.Send(pkt);
                //AVFrame *frame = adecode.Recv();
                //cout << "Audio:" << frame << endl;
            }
            else
            {
                vdecode.Send(pkt);
                AVFrame *frame = vdecode.Recv();
                video->Repaint(frame);
                msleep(40);
                cout << "Video:" << frame << endl;
            }
            if (!pkt)break;
        }
    }
    Demux* demux;
    Decoder vdecode;
    Decoder adecode;
    VideoWidget *video;

};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    createActions();
    testThread = new TestThread();
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


    testThread->Init(fn.toLocal8Bit().data());
    ui->videoWidget->Init(testThread->demux->width, testThread->demux->height);
    testThread->video =  ui->videoWidget;
    testThread->start();
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

