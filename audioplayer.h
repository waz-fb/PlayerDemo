#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H


class AudioPlayer
{
public:
    int sampleRate = 44100;
    int channels = 2;

    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual bool Write(const unsigned char *data, int len) = 0;
    virtual bool isWritable(int size) = 0;
    static AudioPlayer *Get();
    virtual ~AudioPlayer();

protected:
     AudioPlayer();
};

#endif // AUDIOPLAYER_H
