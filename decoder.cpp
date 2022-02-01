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

        AVDictionary *opts = NULL;

        //
        av_dict_set(&opts, "rtsp_transport", "tcp", 0);
        av_dict_set(&opts, "max_delay", "500", 0);

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
        return !!codec;
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

bool Decoder::Send(AVPacket *pkt)
{
    //package cannot be empty
    if (!pkt || pkt->size <= 0 || !pkt->data) return false;
    mux.lock();
    if (!codec)
    {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, pkt);
    mux.unlock();
    av_packet_free(&pkt);
    if (re != 0)return false;
    return true;
}

AVFrame* Decoder::Recv()
{
    mux.lock();
    if (!codec)
    {
        mux.unlock();
        return NULL;
    }
    AVFrame *frame = av_frame_alloc();
    int re = avcodec_receive_frame(codec, frame);
    mux.unlock();
    if (re != 0)
    {
        av_frame_free(&frame);
        return NULL;
    }
    //cout << "["<<frame->linesize[0] << "] " << flush;
    return frame;
}
