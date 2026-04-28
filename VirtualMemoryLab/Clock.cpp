#include "Clock.h"

Clock::Clock(int frameCount) {
    this->frameCount = frameCount;
    clockHand = 0;

    for (int i = 0; i < frameCount; i++) {
        frames.push_back({-1, -1, false});
    }
}

void Clock::markReferenced(int pid, int pageNumber) {
    for (auto& frame : frames) {
        if (frame.pid == pid && frame.pageNumber == pageNumber) {
            frame.referenced = true;
            return;
        }
    }
}

int Clock::evictAndReplace(int pid, int pageNumber, int& evictedPage) {
    while (true) {
        if (frames[clockHand].pid == -1) {
            int frameNumber = clockHand;
            evictedPage = -1;

            frames[clockHand] = {pid, pageNumber, true};
            clockHand = (clockHand + 1) % frameCount;

            return frameNumber;
        }

        if (frames[clockHand].referenced == false) {
            int frameNumber = clockHand;
            evictedPage = frames[clockHand].pageNumber;

            frames[clockHand] = {pid, pageNumber, true};
            clockHand = (clockHand + 1) % frameCount;

            return frameNumber;
        }

        frames[clockHand].referenced = false;
        clockHand = (clockHand + 1) % frameCount;
    }
}

void Clock::printFrames() const {
    std::cout << "\nFrame Table:\n";

    for (int i = 0; i < frameCount; i++) {
        std::cout << "Frame " << i
                  << " | PID: " << frames[i].pid
                  << " | Page: " << frames[i].pageNumber
                  << " | Ref: " << frames[i].referenced
                  << std::endl;
    }
}
