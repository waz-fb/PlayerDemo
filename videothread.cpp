
#include "videothread.h"
#include "decoder.h"
#include "ivideocall.h"

#include <iostream>
using namespace std;

bool VideoThread::Open(AVCodecParameters *para, IVideoCall *call,int width,int height)
{
    if (!para)return false;
    mux.lock();

    this->call = call;
    if (call)
    {
        call->Init(width, height);
    }

    if (!decoder) decoder = new Decoder();
    int re = true;
    if (!decoder->Open(para))
    {
        cout << "video Decoder open failed!" << endl;
        re = false;
    }
    mux.unlock();
    cout << "XVideoThread::Open :" << re << endl;
    return re;
}

void VideoThread::Push(AVPacket *pkt)
{
    if (!pkt)return;
    while (!isExit)
    {
        mux.lock();
        if (packs.size() < maxList)
        {
            packs.push_back(pkt);
            mux.unlock();
            break;
        }
        mux.unlock();
        msleep(1);
    }
}

void VideoThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if (packs.empty() || !decoder)
        {
            mux.unlock();
            msleep(1);
            continue;
        }

        AVPacket *pkt = packs.front();
        packs.pop_front();
        bool re = decoder->Send(pkt);
        if (!re)
        {
            mux.unlock();
            msleep(1);
            cout << "cannot send" << endl;
            continue;
        }
        while (!isExit)
        {
            AVFrame * frame = decoder->Recv();
            if (!frame) break;
            if (call)
            {
                call->Repaint(frame);
            }

        }
        mux.unlock();
    }
}

VideoThread::~VideoThread()
{
    isExit = true;
    wait();
}
