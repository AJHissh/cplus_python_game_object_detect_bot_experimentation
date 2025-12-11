#include "screen_capture.h"
#include <windows.h>
#include <iostream>

ScreenCapture::ScreenCapture() 
    : hwnd_(nullptr), hwindow_dc_(nullptr), hwindow_compatible_dc_(nullptr),
      hbitmap_(nullptr), hbitmap_old_(nullptr), width_(0), height_(0) {
}

ScreenCapture::~ScreenCapture() {
    cleanup();
}

bool ScreenCapture::initialize() {
    hwnd_ = GetDesktopWindow();
    hwindow_dc_ = GetDC((HWND)hwnd_);
    hwindow_compatible_dc_ = CreateCompatibleDC((HDC)hwindow_dc_);
    SetStretchBltMode((HDC)hwindow_compatible_dc_, COLORONCOLOR);
    
    if (!hwindow_dc_ || !hwindow_compatible_dc_) {
        std::cerr << "Failed to initialize screen capture" << std::endl;
        return false;
    }
    
    return true;
}

bool ScreenCapture::capture_region(int x, int y, int width, int height, cv::Mat& output) {
    if (!hwindow_dc_ || !hwindow_compatible_dc_) {
        return false;
    }
    
    // Create bitmap
    HBITMAP hbitmap = CreateCompatibleBitmap((HDC)hwindow_dc_, width, height);
    if (!hbitmap) {
        return false;
    }
    
    // Select bitmap into compatible DC
    HBITMAP hbitmap_old = (HBITMAP)SelectObject((HDC)hwindow_compatible_dc_, hbitmap);
    
    // Copy screen to bitmap
    BOOL result = BitBlt((HDC)hwindow_compatible_dc_, 0, 0, width, height, 
                        (HDC)hwindow_dc_, x, y, SRCCOPY);
    
    if (!result) {
        SelectObject((HDC)hwindow_compatible_dc_, hbitmap_old);
        DeleteObject(hbitmap);
        return false;
    }
    
    // Configure bitmap info
    BITMAPINFOHEADER bi;
    memset(&bi, 0, sizeof(BITMAPINFOHEADER));
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;  // Negative for top-down DIB
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    
    // Create Mat and get bitmap bits
    output.create(height, width, CV_8UC4);
    
    int ret = GetDIBits((HDC)hwindow_compatible_dc_, hbitmap, 0, height,
                       output.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    
    if (ret == 0) {
        output.release();
        SelectObject((HDC)hwindow_compatible_dc_, hbitmap_old);
        DeleteObject(hbitmap);
        return false;
    }
    
    // Convert BGRA to BGR (remove alpha channel)
    cv::Mat bgr_frame;
    cv::cvtColor(output, bgr_frame, cv::COLOR_BGRA2BGR);
    output = bgr_frame;
    
    // Cleanup
    SelectObject((HDC)hwindow_compatible_dc_, hbitmap_old);
    DeleteObject(hbitmap);
    
    return true;
}

void ScreenCapture::cleanup() {
    if (hwindow_compatible_dc_) {
        DeleteDC((HDC)hwindow_compatible_dc_);
    }
    if (hwindow_dc_ && hwnd_) {
        ReleaseDC((HWND)hwnd_, (HDC)hwindow_dc_);
    }
    
    hwindow_compatible_dc_ = nullptr;
    hwindow_dc_ = nullptr;
    hwnd_ = nullptr;
}