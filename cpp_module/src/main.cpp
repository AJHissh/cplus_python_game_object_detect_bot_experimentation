#include "screen_capture.h"
#include "mouse_controller.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class BotController {
public:
    BotController() : screen_capture_(nullptr), mouse_controller_(nullptr) {}
    
    bool initialize() {
        screen_capture_ = new ScreenCapture();
        mouse_controller_ = new MouseController();
        return screen_capture_->initialize();
    }
    
    ~BotController() {
        if (screen_capture_) {
            screen_capture_->cleanup();
            delete screen_capture_;
        }
        if (mouse_controller_) {
            delete mouse_controller_;
        }
    }
    
    py::array_t<uint8_t> capture_region(int x, int y, int width, int height) {
        cv::Mat frame;
        if (screen_capture_ && screen_capture_->capture_region(x, y, width, height, frame)) {
            // Create a copy and return as numpy array
            auto result = py::array_t<uint8_t>({frame.rows, frame.cols, frame.channels()});
            auto buffer = result.mutable_data();
            std::memcpy(buffer, frame.data, frame.total() * frame.elemSize());
            return result;
        }
        return py::array_t<uint8_t>();
    }
    
    bool move_mouse(int x, int y) {
        return mouse_controller_ ? mouse_controller_->move_mouse(x, y) : false;
    }
    
    bool click() {
        return mouse_controller_ ? mouse_controller_->click() : false;
    }
    
    std::tuple<int, int> get_screen_resolution() {
        int width = 0, height = 0;
        if (mouse_controller_) {
            mouse_controller_->get_screen_resolution(width, height);
        }
        return std::make_tuple(width, height);
    }

private:
    ScreenCapture* screen_capture_;
    MouseController* mouse_controller_;
};

PYBIND11_MODULE(bot_controller, m) {
    m.doc() = "Bot controller for monster hunting";
    
    py::class_<BotController>(m, "BotController")
        .def(py::init<>())
        .def("initialize", &BotController::initialize)
        .def("capture_region", &BotController::capture_region)
        .def("move_mouse", &BotController::move_mouse)
        .def("click", &BotController::click)
        .def("get_screen_resolution", &BotController::get_screen_resolution);
}