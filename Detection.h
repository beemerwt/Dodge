#pragma once

class Detection {
public:
    Detection(int& refKeybind) : keybind(refKeybind) {
        timelast = std::chrono::high_resolution_clock::now();
    }

    void Process(std::vector<cv::Vec3f>& circles);
    void SetDodgeCallback(std::function<void()> callback) {
        m_dodgeCallback = callback;
    }

private:
    void Dodge();
    void Predict(float elapsed);
    void Reset();

private:
    std::function<void()> m_dodgeCallback;
    std::chrono::high_resolution_clock::time_point timelast;

    int& keybind;

    float minRadius = 10.0f; // minimum radius to trigger a dodge

    float lastRadius = 0.0f;
    float delta = 0.0f; // rate of radius change

    int framesDetected = 0;
    bool wasDetected = false;
};