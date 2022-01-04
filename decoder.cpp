#include "decoder.h"
extern "C"
{
#include<libavcodec/avcodec.h>
}
#include <iostream>

using namespace std;

Decoder::Decoder()
{

}


Decoder::~Decoder(){

}

bool Decoder::Open(AVCodecParameters *parameter){
    if (!parameter) return false;
        Close();

        AVCodec *vcodec = avcodec_find_decoder(parameter->codec_id);
        if (!vcodec)
        {
            avcodec_parameters_free(&parameter);
            cout << "can't find the codec id " << parameter->codec_id << endl;
            return false;
        }
        cout << "find the AVCodec " << parameter->codec_id << endl;

        mux.lock();
        codec = avcodec_alloc_context3(vcodec);

        avcodec_parameters_to_context(codec, parameter);
        avcodec_parameters_free(&parameter);


        codec->thread_count = 2;

        int re = avcodec_open2(codec, 0, 0);
        if (re != 0)
        {
            avcodec_free_context(&codec);
            mux.unlock();
            char buf[1024] = { 0 };
            av_strerror(re, buf, sizeof(buf) - 1);
            cout << "avcodec_open2  failed! :" << buf << endl;
            return false;
        }
        mux.unlock();
        cout << " avcodec_open2 success!" << endl;
        return true;
}

void Decoder::Close() {
    mux.lock();
    if (codec)
    {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    mux.unlock();

}

void Decoder::Clear() {
    mux.lock();
    if (codec)
        avcodec_flush_buffers(codec);

    mux.unlock();
}
