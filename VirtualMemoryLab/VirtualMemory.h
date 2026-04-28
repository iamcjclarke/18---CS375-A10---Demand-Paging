#ifndef VIRTUALMEMORY_H
#define VIRTUALMEMORY_H

#include <vector>
#include <map>
#include <utility>
#include <iostream>

class TLBEntry {
public:
    int pid;
    int pageNumber;
    int frameNumber;
    int lastUsed;
};

class VirtualMemory {
private:
    std::vector<TLBEntry> tlb;
    std::map<std::pair<int, int>, int> outerPageTable;
    std::map<std::pair<int, int>, int> innerPageTable;

    int tlbSize;
    int currentTime;
    int tlbHits;
    int tlbMisses;
    int pageFaults;

public:
    VirtualMemory(int tlbSize, int pageCount);

    bool lookupTLB(int pid, int virtualAddress, int& physicalAddress);
    bool translateAddress(int pid, int virtualAddress, int& physicalAddress);
    void insertTLB(int pid, int pageNumber, int frameNumber);
    void loadPage(int pid, int pageNumber, int frameNumber);
    void printStats() const;
};

#endif
