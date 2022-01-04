#ifndef DEMUX_H
#define DEMUX_H

#include <mutex>
struct AVFormatContext;
struct AVPacket;

class Demux
{
public:
    Demux();
    ~Demux();

    virtual bool Open(const char *url);

    //Caller should relase the memmory
    virtual AVPacket *Read();

    int totalMs = 0;
protected:
    std::mutex mux;
    AVFormatContext *formatContext = NULL;
    int videoStream = 0;
    int audioStream = 1;
};

#endif // DEMUX_H
