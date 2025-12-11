#ifndef SCREEN_CAPTURE_H
#define SCREEN_CAPTURE_H

#include <opencv2/opencv.hpp>

class ScreenCapture {
public:
    ScreenCapture();
    ~ScreenCapture();
    
    bool initialize();
    bool capture_region(int x, int y, int width, int height, cv::Mat& output);
    void cleanup();
    
private:
    void* hwnd_;
    void* hwindow_dc_;
    void* hwindow_compatible_dc_;
    void* hbitmap_;
    void* hbitmap_old_;
    int width_;
    int height_;
};

#endif