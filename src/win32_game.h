#pragma once

#include<windows.h>

#define global_variable static
// localy scoped variable that will persist its value
// when function goes out of scope
#define local_persist static
// reserved for functions that are going to
// be internal to this file, local functions
#define internal static

struct win32_offscreen_buffer
{
    // The BITMAPINFOHEADER structure 
    // contains information about the dimensions 
    // and color format of a DIB. 
    BITMAPINFO info;
    void *memory;
    int width;
    int height;
    // this is the value which you want to add to the index
    // if you want to get to the next row when storing 2d thing in 1d
    int pitch;
    // Note: Pixels are always 32bits wide
};

// TODO: unpack function calls window_dim to width, height 
struct window_dimension
{
    int width;
    int height;
};

