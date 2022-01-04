#ifndef DECODER_H
#define DECODER_H

struct AVCodecParameters;
struct AVCodecContext;
#include <mutex>

class Decoder
{
public:
    Decoder();
    ~Decoder();

    virtual bool Open(AVCodecParameters *parameter);

    virtual void Close();
    virtual void Clear();

protected:
    AVCodecContext *codec = 0;
    std::mutex mux;
};

#endif // DECODER_H
