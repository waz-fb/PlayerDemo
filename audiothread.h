#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>
#include <mutex>
#include <list>

class Decoder;
class Resampler;
class AudioPlayer;
struct AVCodecParameters;
struct AVPacket;

class AudioThread : public QThread
{
public:
    AudioThread():isExit(false){};
    ~AudioThread();
    bool Open(AVCodecParameters* parameters, int32_t sampleRate, int32_t channels);
    void Push(AVPacket* packet);
    void run();
private:
    std::mutex mux;
    Decoder* decoder = nullptr;
    AudioPlayer* player = nullptr;
    Resampler* resampler = nullptr;

    std::list<AVPacket*> packs;
    u_int32_t maxsize = 200;
    bool isExit;
};

#endif // AUDIOTHREAD_H
