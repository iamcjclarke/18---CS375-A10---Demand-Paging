#include "VirtualMemory.h"

VirtualMemory::VirtualMemory(int tlbSize, int pageCount) {
    this->tlbSize = tlbSize;
    currentTime = 0;
    tlbHits = 0;
    tlbMisses = 0;
    pageFaults = 0;
}

bool VirtualMemory::lookupTLB(int pid, int virtualAddress, int& physicalAddress) {
    currentTime++;

    int pageNumber = virtualAddress >> 12;
    int offset = virtualAddress & 0xFFF;

    for (auto& entry : tlb) {
        if (entry.pid == pid && entry.pageNumber == pageNumber) {
            entry.lastUsed = currentTime;
            physicalAddress = (entry.frameNumber << 12) | offset;
            tlbHits++;
            return true;
        }
    }

    tlbMisses++;
    return false;
}

bool VirtualMemory::translateAddress(int pid, int virtualAddress, int& physicalAddress) {
    int pageNumber = virtualAddress >> 12;
    int offset = virtualAddress & 0xFFF;

    int outerPage = (pageNumber >> 10) & 0x3FF;
    int innerPage = pageNumber & 0x3FF;

    std::pair<int, int> outerKey = {pid, outerPage};
    std::pair<int, int> innerKey = {pid, pageNumber};

    if (innerPageTable.find(innerKey) == innerPageTable.end()) {
        pageFaults++;
        return false;
    }

    int frameNumber = innerPageTable[innerKey];
    physicalAddress = (frameNumber << 12) | offset;

    insertTLB(pid, pageNumber, frameNumber);

    return true;
}

void VirtualMemory::insertTLB(int pid, int pageNumber, int frameNumber) {
    currentTime++;

    for (auto& entry : tlb) {
        if (entry.pid == pid && entry.pageNumber == pageNumber) {
            entry.frameNumber = frameNumber;
            entry.lastUsed = currentTime;
            return;
        }
    }

    if ((int)tlb.size() < tlbSize) {
        tlb.push_back({pid, pageNumber, frameNumber, currentTime});
        return;
    }

    int lruIndex = 0;

    for (int i = 1; i < (int)tlb.size(); i++) {
        if (tlb[i].lastUsed < tlb[lruIndex].lastUsed) {
            lruIndex = i;
        }
    }

    tlb[lruIndex] = {pid, pageNumber, frameNumber, currentTime};
}

void VirtualMemory::loadPage(int pid, int pageNumber, int frameNumber) {
    int outerPage = (pageNumber >> 10) & 0x3FF;

    std::pair<int, int> outerKey = {pid, outerPage};
    std::pair<int, int> innerKey = {pid, pageNumber};

    outerPageTable[outerKey] = outerPage;
    innerPageTable[innerKey] = frameNumber;

    insertTLB(pid, pageNumber, frameNumber);
}

void VirtualMemory::printStats() const {
    std::cout << "\nSimulation Statistics:\n";
    std::cout << "TLB Hits: " << tlbHits << std::endl;
    std::cout << "TLB Misses: " << tlbMisses << std::endl;
    std::cout << "Page Faults: " << pageFaults << std::endl;

    int total = tlbHits + tlbMisses;

    if (total > 0) {
        double hitRate = (double)tlbHits / total * 100.0;
        std::cout << "TLB Hit Rate: " << hitRate << "%" << std::endl;
    }
}
