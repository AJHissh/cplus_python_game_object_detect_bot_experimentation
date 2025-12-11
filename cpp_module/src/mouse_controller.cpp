#include "mouse_controller.h"
#include <windows.h>
#include <iostream>

MouseController::MouseController() {
    screen_width_ = GetSystemMetrics(SM_CXSCREEN);
    screen_height_ = GetSystemMetrics(SM_CYSCREEN);
}

bool MouseController::move_mouse(int x, int y) {
    try {
        // Normalize coordinates
        int normalized_x = (x * 65535) / screen_width_;
        int normalized_y = (y * 65535) / screen_height_;
        
        // Use absolute positioning
        INPUT input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
        input.mi.dx = normalized_x;
        input.mi.dy = normalized_y;
        
        return SendInput(1, &input, sizeof(INPUT)) == 1;
    }
    catch (...) {
        return false;
    }
}

bool MouseController::click() {
    try {
        INPUT inputs[2] = {0};
        
        // Mouse down
        inputs[0].type = INPUT_MOUSE;
        inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        
        // Mouse up
        inputs[1].type = INPUT_MOUSE;
        inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
        
        return SendInput(2, inputs, sizeof(INPUT)) == 2;
    }
    catch (...) {
        return false;
    }
}

bool MouseController::get_screen_resolution(int& width, int& height) {
    width = screen_width_;
    height = screen_height_;
    return true;
}