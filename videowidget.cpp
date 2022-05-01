#include "videowidget.h"

#include <QOpenGLShader>
#include <QDebug>
#include <QTimer>
#include <QOpenGLExtraFunctions>

#define STR(x) #x
#define A_VER 3
#define T_VER 4

extern "C" {
#include <libavutil/frame.h>
}

FILE *fp = NULL;

// vertext shader
const char *vString = STR(
    attribute vec4 vertexIn;
    attribute vec2 textureIn;
    varying vec2 textureOut;
    void main(void)
    {
        gl_Position = vertexIn;
        textureOut = textureIn;
    }
);


//fragment shader
const char *tString = STR(
    varying vec2 textureOut;
    uniform sampler2D tex_y;
    uniform sampler2D tex_u;
    uniform sampler2D tex_v;
    void main(void)
    {
        vec3 yuv;
        vec3 rgb;
        yuv.x = texture2D(tex_y, textureOut).r;
        yuv.y = texture2D(tex_u, textureOut).r - 0.5;
        yuv.z = texture2D(tex_v, textureOut).r - 0.5;
        rgb = mat3(
            1.0, 1.0, 1.0,
            0.0, -0.39465, 2.03211,
            1.13983, -0.58060, 0.0) * yuv;
        gl_FragColor = vec4(rgb, 1.0);
    }
);



VideoWidget::VideoWidget(QWidget *parent):QOpenGLWidget(parent){}

VideoWidget::~VideoWidget(){}

void VideoWidget::Init(int width, int height) {
    mux.lock();

    // release data and textures if previously exists.
    delete datas[0];
    delete datas[1];
    delete datas[2];
    if (texs[0]) {
        glDeleteTextures(3, texs);
    }

    // init
    this->width = width;
    this->height = height;
    datas[0] = new unsigned char[width*height];		// Y
    datas[1] = new unsigned char[width*height/4];	// U
    datas[2] = new unsigned char[width*height/4];   // V



    glGenTextures(3, texs);

    glBindTexture(GL_TEXTURE_2D, texs[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);


    glBindTexture(GL_TEXTURE_2D, texs[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width/2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, texs[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    qDebug() << "initilize widget" << width <<" , " << height;

    mux.unlock();
}


void VideoWidget::Repaint(AVFrame *frame)
{
    qDebug() << "Repaint";
    if (!frame)return;
    mux.lock();
    if (!datas[0] || width*height == 0 || frame->width != this->width || frame->height != this->height)
    {
        av_frame_free(&frame);
        mux.unlock();
        return;
    }

    memcpy(datas[0], frame->data[0], width*height);
    memcpy(datas[1], frame->data[1], width*height/4);
    memcpy(datas[2], frame->data[2], width*height/4);
    mux.unlock();

    update();
}

void VideoWidget::initializeGL()
{
    qDebug() << "initializeGL";

    mux.lock();
    initializeOpenGLFunctions();
    qDebug()<< program.addShaderFromSourceCode(QOpenGLShader::Fragment, tString);
    qDebug() << program.addShaderFromSourceCode(QOpenGLShader::Vertex, vString);

    program.bindAttributeLocation("vertexIn",A_VER);
    program.bindAttributeLocation("textureIn",T_VER);
    qDebug() << "program.link() = " << program.link();
    qDebug() << "program.bind() = " << program.bind();


     static const GLfloat ver[] = {
            -1.0f,-1.0f,
            1.0f,-1.0f,
            -1.0f, 1.0f,
            1.0f, 1.0f
        };

      static const GLfloat tex[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };

     glEnableVertexAttribArray(0);
     glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
     glEnableVertexAttribArray(A_VER);

     glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
     glEnableVertexAttribArray(T_VER);

     unis[0] = program.uniformLocation("tex_y");
     unis[1] = program.uniformLocation("tex_u");
     unis[2] = program.uniformLocation("tex_v");

     mux.unlock();
}

void VideoWidget::paintGL()
{
   //qDebug() << "paintGL";
    mux.lock();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texs[0]);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
    glUniform1i(unis[0], 0);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, texs[1]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width/2, height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
    glUniform1i(unis[1],1);


    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, texs[2]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
    glUniform1i(unis[2], 2);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    mux.unlock();

}

void VideoWidget::resizeGL(int width, int height)
{
    qDebug() << "resizeGL "<<width<<":"<<height;
}
