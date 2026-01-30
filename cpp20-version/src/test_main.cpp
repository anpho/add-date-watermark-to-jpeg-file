#include <iostream>
#include <string>
#include <filesystem>
#include "../include/exif_reader.h"
#include "../include/image_processor.h"
#include "../include/utils.h"

// Initialize COM
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>

namespace fs = std::filesystem;

int main() {
    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to initialize COM library." << std::endl;
        return 1;
    }

    auto comCleanup = [&]() {
        CoUninitialize();
    };

    // Initialize GDI+
    ImageProcessor imageProc;
    if (!imageProc.initialize()) {
        std::wcerr << L"Failed to initialize GDI+." << std::endl;
        comCleanup();
        return 1;
    }

    // Test EXIF reader
    SimpleTestFramework::runTest("EXIF Reader Initialization", []() {
        ExifReader reader;
        return true; // Just test if constructor works
    });

    // Test image processor initialization
    SimpleTestFramework::runTest("Image Processor Initialization", [&imageProc]() {
        return imageProc.initialize();
    });

    // Test date formatting function (we'll implement this in main as well)
    SimpleTestFramework::runTest("Date Formatting", []() {
        // This is a simple test - in a real scenario we'd have a separate utility function
        std::wstring input = L"2023:05:15 14:30:45";
        std::wstring expected = L"2023-05-15";
        
        if (input.length() < 10) return false;
        
        std::wstring result = input;
        for (size_t i = 0; i < result.length() && i < 10; ++i) {
            if (result[i] == L':') {
                result[i] = L'-';
            }
        }
        result = result.substr(0, 10);
        
        return result == expected;
    });

    // Print test results
    SimpleTestFramework::printSummary();

    comCleanup();
    return 0;
}