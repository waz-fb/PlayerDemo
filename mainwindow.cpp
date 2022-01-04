#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMenuBar"
#include "decoder.h"

#include <QContextMenuEvent>
#include <QFileDialog>


#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    createActions();
    demux = new Demux();
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
    demux->Open(fn.toLocal8Bit().data());
    Decoder vdecoder;
    cout << "vdecoder.Open() = " << vdecoder.Open(demux->getVideoParameter()) << endl;
    vdecoder.Clear();
    vdecoder.Close();
    Decoder adecoder;
    cout << "adecode.Open() = " << adecoder.Open(demux->getAudioParameter()) << endl;


    for (;;)
        {
            AVPacket *pkt = demux->Read();
            if (!pkt)break;
        }


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

