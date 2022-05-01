#pragma once

struct AVPacket;
struct AVCodecParameters;
class Decoder;
class IVideoCall;

#include <list>
#include <mutex>
#include <QThread>


class VideoThread: public QThread
{
public:
    virtual bool Open(AVCodecParameters *para,IVideoCall *call,int width,int height);
    virtual void Push(AVPacket *pkt);
    void run();
    virtual ~VideoThread();
    VideoThread():isExit(false){}
    u_int32_t maxList = 100;
    bool isExit;
protected:
    std::list <AVPacket *> packs;
    std::mutex mux;
    Decoder *decoder = 0;
    IVideoCall *call = 0;
};
