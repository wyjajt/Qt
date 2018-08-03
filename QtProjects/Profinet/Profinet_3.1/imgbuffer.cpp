#include "imgbuffer.h"

ImgBuffer::ImgBuffer()
{
//    pbyBuffer       = NULL;
    pbImgBuffer     = (unsigned char*)malloc( 1280 * 1024 * 3 );
    is_idle         = true;
    bufferSize      = 1280 * 1024;
    bufferChannel   = 3;
}

ImgBuffer::ImgBuffer( int width, int height, int channel )
{
//    pbyBuffer   = NULL;
    pbImgBuffer     = (unsigned char*)malloc( width * height * channel );
    is_idle         = true;
    bufferSize      = width * height;
    bufferChannel   = channel;
}
