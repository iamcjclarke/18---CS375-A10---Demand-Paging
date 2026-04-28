#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "VirtualMemory.h"
#include "Clock.h"

using namespace std;

int main() {
    const int TLB_SIZE = 8;
    const int FRAME_COUNT = 16;

    VirtualMemory vm(TLB_SIZE, FRAME_COUNT);
    Clock clock(FRAME_COUNT);

    ifstream inputFile("input/trace.txt");

    if (!inputFile) {
        cout << "Error: could not open input/trace.txt" << endl;
        return 1;
    }

    string line;

    cout << "Virtual Memory Simulation Started\n";

    while (getline(inputFile, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        stringstream ss(line);

        int pid;
        string addressString;

        ss >> pid >> addressString;

        int virtualAddress = stoi(addressString, nullptr, 16);
        int physicalAddress;

        cout << "\nPID " << pid << " requests virtual address " << addressString << endl;

        if (vm.lookupTLB(pid, virtualAddress, physicalAddress)) {
            cout << "TLB Hit" << endl;
            cout << "Physical Address: 0x" << hex << physicalAddress << dec << endl;
        } 
        else {
            cout << "TLB Miss" << endl;

            if (vm.translateAddress(pid, virtualAddress, physicalAddress)) {
                cout << "Page found in page table" << endl;
                cout << "Physical Address: 0x" << hex << physicalAddress << dec << endl;
            } 
            else {
                cout << "Page Fault" << endl;

                int pageNumber = virtualAddress >> 12;
                int evictedPage;

                int frameNumber = clock.evictAndReplace(pid, pageNumber, evictedPage);

                if (evictedPage != -1) {
                    cout << "Evicted Page: " << evictedPage << endl;
                } 
                else {
                    cout << "Used empty frame" << endl;
                }

                vm.loadPage(pid, pageNumber, frameNumber);

                int offset = virtualAddress & 0xFFF;
                physicalAddress = (frameNumber << 12) | offset;

                cout << "Loaded page " << pageNumber << " into frame " << frameNumber << endl;
                cout << "Physical Address: 0x" << hex << physicalAddress << dec << endl;
            }
        }
    }

    vm.printStats();
    clock.printFrames();

    return 0;
}
