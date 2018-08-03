#ifndef IMGBUFFER_H
#define IMGBUFFER_H

//#include "CameraApi.h"
#include <Windows.h>

//buffer结构体
class ImgBuffer
{
public:
//    BYTE    *pbyBuffer;
    BYTE    *pbImgBuffer;
    bool    is_idle;     //内存处于空闲状态
    int     bufferSize;
    int     bufferChannel;

public:
    //默认构造
    ImgBuffer();
    ImgBuffer( int width, int height, int channel );

};
//Q_DECLARE_METATYPE(ImgBuffer);

#endif // IMGBUFFER_H
