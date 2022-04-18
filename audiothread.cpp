#include "audiothread.h"
#include "decoder.h"
#include "audioplayer.h"
#include "resampler.h"
#include "iostream"

using namespace std;
extern "C"
{
#include<libavcodec/avcodec.h>
}

AudioThread::~AudioThread() {
    isExit = true;
    wait();
}

bool AudioThread::Open(AVCodecParameters* parameters,  int32_t sampleRate, int32_t channels){
    if (!parameters) {
        return false;
    }
    mux.lock();
    if(!decoder)
        decoder = new Decoder();
    if(!player)
        player = AudioPlayer::Get();
    if(!resampler)
        resampler = new Resampler();
    mux.unlock();

    AVCodecParameters* temp = avcodec_parameters_alloc();
    avcodec_parameters_copy(temp, parameters);
    bool re = true;
    if(!decoder->Open(temp)) {
        cout << "audio XDecode open failed!" << endl;
       re = false;
    }

    if (!resampler->Open(parameters)) {
        cout << "XResample open failed!" << endl;
        re = false;
    }
    player->channels = channels;
    player->sampleRate = sampleRate;

    if (!player->Open()) {
        cout << "XAudioPlay open failed!" << endl;
        mux.unlock();
        return false;
    }
    mux.unlock();
    return re;
}

void AudioThread::Push(AVPacket* packet) {
    if(!packet) return;
        while (!isExit) {
            mux.lock();
            if (packs.size() < maxsize) {
                packs.push_back(packet);
                mux.unlock();
                break;
            }
            mux.unlock();
            msleep(1);
        }

}
void AudioThread::run() {
    unsigned char *pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit) {
        mux.lock();
        if (packs.empty() || !decoder || !resampler || !player) {
            mux.unlock();
            msleep(1);
            continue;
        }

        AVPacket* packet = packs.front();
        packs.pop_front();
        bool re = decoder->Send(packet);
        if (!re) {
            mux.unlock();
            msleep(1);
            cout << "canot send" << endl;
            continue;
        }
        // Can recive multiple Frame with signle send
        while(!isExit) {
            AVFrame* frame = decoder->Recv();
            if(!frame) {
                break;
            }
            int size = resampler->Resample(frame, pcm);
            if (size <= 0) continue;
            while(!isExit) {
                if (player->isWritable(size)) {
                    player->Write(pcm, size);
                    break;
                }
                msleep(1);
                continue;

            }
        }
        mux.unlock();
    }
    delete[] pcm;
}
