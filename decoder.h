#ifndef DECODER_H
#define DECODER_H

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
#include <mutex>

class Decoder
{
public:

    virtual bool Open(AVCodecParameters *parameter);

    //send packet to decoder thread, release the packet space whatever success or not
    virtual bool Send(AVPacket *pkt);

    //get decode result, may recieve multiple frames
    virtual AVFrame* Recv();

    virtual void Close();
    virtual void Clear();

    bool isReady() {
        return !!codec;
    }

protected:
    AVCodecContext *codec = 0;
    std::mutex mux;
};

#endif // DECODER_H
