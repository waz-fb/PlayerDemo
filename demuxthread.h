#ifndef DEMUXTHREAD_H
#define DEMUXTHREAD_H

#include <QThread>
#include <mutex>

class Demux;
class VideoThread;
class AudioThread;
class IVideoCall;

class DemuxThread: public QThread
{
private:
    std::mutex mux;
    VideoThread* vt = nullptr;
    AudioThread* at = nullptr;
    Demux* demux = nullptr;

public:
    ~DemuxThread();
    bool Open(const char* Open, IVideoCall* call);
    void Start();
    void run();
    bool isExit = false;

};

#endif // DEMUXTHREAD_H
