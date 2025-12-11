#ifndef MOUSE_CONTROLLER_H
#define MOUSE_CONTROLLER_H

class MouseController {
public:
    MouseController();
    
    bool move_mouse(int x, int y);
    bool click();
    bool get_screen_resolution(int& width, int& height);
    
private:
    int screen_width_;
    int screen_height_;
};

#endif