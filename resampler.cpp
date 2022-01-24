#include "resampler.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
#include <iostream>
using namespace std;

void Resampler::Close()
{
    mux.lock();
    if (actx)
        swr_free(&actx);

    mux.unlock();
}

bool Resampler::Open(AVCodecParameters *para)
{
    if (!para)return false;
    mux.lock();

    actx = swr_alloc_set_opts(actx,
        av_get_default_channel_layout(2),
        static_cast<AVSampleFormat>(outFormat),
        para->sample_rate,
        av_get_default_channel_layout(para->channels),
        static_cast<AVSampleFormat>(para->format),
        para->sample_rate,
        0, 0
    );
    avcodec_parameters_free(&para);
    int re = swr_init(actx);
    mux.unlock();
    if (re != 0)
    {
        char buf[1024] = { 0 };
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "swr_init  failed! :" << buf << endl;
        return false;
    }
    return true;
}

int Resampler::Resample(AVFrame *indata, unsigned char *pcm)
{
    if (!indata) return 0;
    if (!pcm)
    {
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *pcm_wrapper[2] = { 0 };
    pcm_wrapper[0] = pcm;
    int re = swr_convert(actx,
        pcm_wrapper, indata->nb_samples,
        const_cast<const uint8_t**>(indata->data), indata->nb_samples
    );
    if (re <= 0)return re;
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);
    return outSize;
}

Resampler::Resampler()
{
}


Resampler::~Resampler()
{
}
