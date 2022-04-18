#include "audioplayer.h"

#include <QtMultimedia>
#include <QAudioFormat>
#include <QAudioSink>
#include <QAudioDevice>
#include <mutex>
#include <QDebug>

class QtAudioPlayer :public AudioPlayer
{
public:
    QtAudioPlayer(){};

    virtual void Close()
    {
        mux.lock();
        if (io)
        {
            io->close ();
            io = NULL;
         }
         if (audio)
         {
             delete audio;
             audio = 0;
          }
          mux.unlock();
    }
    virtual bool Open()
    {
        Close();
        QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setSampleFormat(QAudioFormat::Int16);
        fmt.setChannelCount(channels);
        mux.lock();
        audio = new QAudioSink(QMediaDevices::defaultAudioOutput(), fmt);
        io =  audio->start();
        mux.unlock();
        if(io)
            return true;
        return false;
    }
    virtual bool Write(const unsigned char *data, int len)
        {
            if (!data || len <= 0) return false;
            mux.lock();
            if (!audio || !io)
            {
                mux.unlock();
                return false;
            }
            int size = io->write((char *)data, len);
            mux.unlock();
            if (len != size)
                return false;
            return true;
        }

        virtual bool isWritable(int size)
        {
            mux.lock();
            if (!audio)
            {
                mux.unlock();
                return 0;
            }
            int free = audio->bytesFree();
            mux.unlock();
            return free > size;
        }
private:
    QAudioSink *audio = NULL;
    QIODevice *io = NULL;
    std::mutex mux;
};

AudioPlayer *AudioPlayer::Get()
{
    static QtAudioPlayer player;
    return &player;
}

AudioPlayer::AudioPlayer()
{

}

AudioPlayer::~AudioPlayer(){

}
