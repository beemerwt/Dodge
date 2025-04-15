#include "pch.h"
#include "Detection.h"

void Detection::Reset() {
    if (framesDetected > 0)
        OutputDebugStringA("Didn't detect third frame. Resetting\n");

    framesDetected = 0;
    wasDetected = false;
    lastRadius = 0.0f;
    delta = 0.0f;
}

void Detection::Predict(float elapsed) {
    // Predict the dodge based on the last known radius and rate of change
    // Use the last known radius and subtract the rate of change to predict the dodge

    float predictedRadius = lastRadius - delta * elapsed;
    lastRadius = predictedRadius;

    std::string output = "Predicted Radius: " + std::to_string(predictedRadius) + "\n";
    OutputDebugStringA(output.c_str());

    if (predictedRadius <= minRadius)
        Dodge();
}

void Detection::Dodge() {
    OutputDebugStringA("Dodging\n");

    // Press the keybind to dodge
    keybd_event(keybind, 0, 0, 0);
    Sleep(33); // Sleep for 33ms to simulate key press duration
    keybd_event(keybind, 0, KEYEVENTF_KEYUP, 0);
    Reset();
}

void Detection::Process(std::vector<cv::Vec3f>& circles) {
    // The circle will always shrink
    // Want to ensure at least 3 consecutive frames were detected
    // Want to detect the rate of change so that we don't have to rely on the radius
    // When the radius is below a certain point we want to trigger a dodge
    // 
    // If it was detected and is no longer detected
    //  we use the last known radius
    //  and subtract the rate of change to predict the dodge

    auto timenow = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration<float>(timenow - timelast).count();
    timelast = timenow;

    if (elapsed > 20) {
        OutputDebugStringA("Frame took longer than 20ms\n");
    }

    if (circles.empty()) {
        if (wasDetected) {
            Predict(elapsed);
            return;
        }

        Reset();
        return;
    }

    // Ensure that the radius is always shrinking
    float currentRadius = fabsf(circles[0][2]);
    if (lastRadius < currentRadius && lastRadius > 0.0f) {
        OutputDebugStringA("Radius was found but not shrinking\n");
        Reset();
        return;
    }

    // Debug output the radius and rate of change
    std::string debugOutput = "Radius: " + std::to_string(currentRadius) + "\n";
    OutputDebugStringA(debugOutput.c_str());

    // Calculate the rate of change
    if (lastRadius > 0.0f) {
        delta = (lastRadius - currentRadius) / elapsed;
        std::string debugDelta = "Delta: " + std::to_string(delta) + "\n";
        OutputDebugStringA(debugDelta.c_str());
    }
    else {
        OutputDebugStringA("Detected first frame of dodge circle.\n");
    }

    framesDetected += 1;
    wasDetected = framesDetected >= 3;
    lastRadius = currentRadius;
    timelast = timenow;
}