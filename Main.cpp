#include <iostream>
#include "DiscMaster.h"

int main() {
    CoInitialize(nullptr); // COM kitaplığını başlat

    DiscMaster discMaster;

    if (!discMaster.initialize()) {
        std::wcerr << L"Initialization failed: " << discMaster.getError().GetString() << std::endl;
        return 1;
    }

    std::wcout << L"Initialization succeeded." << std::endl;

    long deviceCount = discMaster.getNumberOfDevices();
    std::wcout << L"Number of devices: " << deviceCount << std::endl;

    if (!discMaster.enumerateDiscMasters()) {
        std::wcerr << L"Enumeration failed: " << discMaster.getError().GetString() << std::endl;
        return 1;
    }


    std::wcout << L"Enumeration succeeded." << std::endl;

    CoUninitialize(); // COM kitaplığını kapat
    return 0;
}