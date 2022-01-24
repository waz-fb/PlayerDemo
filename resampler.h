#ifndef RESAMPLER_H
#define RESAMPLER_H

struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
#include <mutex>

class Resampler
{
public:

    //use s16, para will be free after use
    virtual bool Open(AVCodecParameters *para);
    virtual void Close();

    // resample the indata, will relesae it whatever suceess or not
    virtual int Resample(AVFrame *indata, unsigned char *data);

    Resampler();
    ~Resampler();

    //AV_SAMPLE_FMT_S16
    int outFormat = 1;
protected:
    std::mutex mux;
    SwrContext *actx = 0;
};


#endif // RESAMPLER_H
