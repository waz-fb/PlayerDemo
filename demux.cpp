#include "demux.h"
#include "iostream"
using namespace std;

extern "C" {
    #include"libavformat/avformat.h"
    #include "libavutil/avutil.h"
    #include "libavcodec/avcodec.h"
}

static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}


Demux::Demux()
{
}

Demux::~Demux() {
    Close();
    cout<<"closed"<<endl;
}

bool Demux::Open(const char* url){
        AVDictionary *opts = NULL;

        mux.lock();
        int re = avformat_open_input(&formatContext,url,0,&opts);
        if (re != 0)
        {
            mux.unlock();
            char buf[1024] = { 0 };
            av_strerror(re, buf, sizeof(buf) - 1);
            cout << "open " << url << " failed! :" << buf << endl;
            return false;
        }
        cout << "open " << url << " success! " << endl;


        avformat_find_stream_info(formatContext, 0);
        int totalMs = formatContext->duration / (AV_TIME_BASE / 1000);
        cout << "totalMs = " << totalMs << endl;


        av_dump_format(formatContext, 0, url, 0);

        cout << "\n=======================================================" << endl;
        videoStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        AVStream *as = formatContext->streams[videoStream];
        cout << videoStream << " video info" << endl;
        cout << "codec_id = " << as->codecpar->codec_id << endl;
        cout << "format = " << as->codecpar->format << endl;
        cout << "width=" << as->codecpar->width << endl;
        cout << "height=" << as->codecpar->height << endl;
        cout << "video fps = " << r2d(as->avg_frame_rate) << endl;

        cout << "\n=======================================================" << endl;
        cout << audioStream << " audio info" << endl;
        audioStream = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
        as = formatContext->streams[audioStream];
        cout << "codec_id = " << as->codecpar->codec_id << endl;
        cout << "format = " << as->codecpar->format << endl;
        cout << "sample_rate = " << as->codecpar->sample_rate << endl;
        //AVSampleFormat;
        cout << "channels = " << as->codecpar->channels << endl;
        cout << "frame_size = " << as->codecpar->frame_size << endl;
        //1024 * 2 * 2 = 4096  fps = sample_rate/frame_size
        mux.unlock();
        return true;
}

AVPacket* Demux::Read()
{
    mux.lock();
    if (!formatContext)
    {
        mux.unlock();
        return 0;
    }
    AVPacket *pkt = av_packet_alloc();

    int re = av_read_frame(formatContext, pkt);
    if (re != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return 0;
    }

    // trasnlate the time to ms
    pkt->pts = pkt->pts*(1000 * (r2d(formatContext->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts*(1000 * (r2d(formatContext->streams[pkt->stream_index]->time_base)));
    mux.unlock();
    cout << pkt->pts << " "<<flush;
    return pkt;

}


// TODO: implement details with decoder
//get video codec parameters, should be free by avcodec_parameters_free
AVCodecParameters* Demux::getVideoParameter() {
    return nullptr;
}

//get video codec parameters, should be free by avcodec_parameters_free
AVCodecParameters* Demux::CopyAPara() {
    return nullptr;
}


bool Demux::Seek(double pos){
    return false;
}

//flush cache
void Demux::Clear(){
    mux.lock();
        if (!formatContext)
        {
            mux.unlock();
            return ;
        }
        avformat_flush(formatContext);
        mux.unlock();

}

void Demux::Close(){
    mux.lock();
        if (!formatContext)
        {
            mux.unlock();
            return;
        }
        avformat_close_input(&formatContext);

        totalMs = 0;
        mux.unlock();
}
