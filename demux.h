#ifndef DEMUX_H
#define DEMUX_H

#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

class Demux
{
public:
    Demux();
    ~Demux();

    virtual bool Open(const char *url);

    //Caller should relase the memmory
    virtual AVPacket *Read();

    //get video codec parameters, should be free by avcodec_parameters_free
    virtual AVCodecParameters *getVideoParameter();

    //get video codec parameters, should be free by avcodec_parameters_free
    virtual AVCodecParameters *CopyAPara();


    virtual bool Seek(double pos);

    //flush cach
    virtual void Clear();
    virtual void Close();

    int totalMs = 0;
protected:
    std::mutex mux;
    AVFormatContext *formatContext = NULL;
    int videoStream = 0;
    int audioStream = 1;
};

#endif // DEMUX_H
