#include "demuxthread.h"
#include "ivideocall.h"
#include "videothread.h"
#include "audiothread.h"
#include "demux.h"
#include <iostream>
using namespace std;

DemuxThread::~DemuxThread() {
    isExit = true;
    wait();
}
bool DemuxThread::Open(const char* url, IVideoCall* call) {
    if (url == 0 || url[0] == '\0')
            return false;

        mux.lock();
        if (!demux) demux = new Demux();
        if (!vt) vt = new VideoThread();
        if (!at) at = new AudioThread();

        bool re = demux->Open(url);
        if (!re)
        {
            cout << "demux->Open(url) failed!" << endl;
            return false;
        }

        if (!vt->Open(demux->getVideoParameter(), call, demux->width, demux->height))
        {
            re = false;
            cout << "vt->Open failed!" << endl;
        }
        if (!at->Open(demux->getAudioParameter(), demux->sampleRate, demux->channels))
        {
            re = false;
            cout << "at->Open failed!" << endl;
        }
        mux.unlock();
        cout << "XDemuxThread::Open " << re << endl;
        return re;
}

void DemuxThread::Start() {
    mux.lock();
    QThread::start();
    if (vt)vt->start();
    if (at)at->start();
    mux.unlock();
}


void DemuxThread::run(){

    while (!isExit)
        {
            mux.lock();
            if (!demux)
            {
                mux.unlock();
                msleep(5);
                continue;
            }
            AVPacket *pkt = demux->Read();
            if (!pkt)
            {
                mux.unlock();
                msleep(5);
                continue;
            }
            if (demux->IsAudio(pkt))
            {
                if(at)at->Push(pkt);
            }
            else
            {
                if (vt)vt->Push(pkt);
            }

            mux.unlock();

        }
}
