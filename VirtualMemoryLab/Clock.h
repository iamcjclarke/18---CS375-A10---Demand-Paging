#ifndef CLOCK_H
#define CLOCK_H

#include <vector>
#include <iostream>

struct FrameEntry {
    int pid;
    int pageNumber;
    bool referenced;
};

class Clock {
private:
    std::vector<FrameEntry> frames;
    size_t clockHand;
    int frameCount;

public:
    Clock(int frameCount);
    int evictAndReplace(int pid, int pageNumber, int& evictedPage);
    void markReferenced(int pid, int pageNumber);
    void printFrames() const;
};

#endif
