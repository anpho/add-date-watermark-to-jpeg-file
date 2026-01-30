#include <iostream>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

#include "exif_reader.h"
#include "image_processor.h"

// Initialize COM
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>

namespace fs = std::filesystem;

/**
 * @brief 打印使用说明
 */
void printUsage() {
    std::wcout << L"Usage: add-date-watermark-cpp20 <input_file> <output_directory>\n";
    std::wcout << L"Example: add-date-watermark-cpp20 D:\\photo.jpg D:\\output\n";
}

/**
 * @brief 验证输入参数
 * @param argc 参数数量
 * @param argv 参数数组
 * @return 是否参数有效
 */
bool validateArguments(int argc, wchar_t* argv[]) {
    if (argc != 3) {
        std::wcerr << L"Invalid number of arguments.\n";
        printUsage();
        return false;
    }

    // Check if input file exists
    fs::path inputPath(argv[1]);
    if (!fs::exists(inputPath)) {
        std::wcerr << L"Input file does not exist: " << argv[1] << std::endl;
        return false;
    }

    // Check if output directory exists
    fs::path outputPath(argv[2]);
    if (!fs::exists(outputPath) || !fs::is_directory(outputPath)) {
        std::wcerr << L"Output directory does not exist: " << argv[2] << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief 从EXIF日期字符串中提取日期部分 (YYYY:MM:DD -> YYYY-MM-DD)
 * @param exifDate EXIF格式的日期字符串
 * @return 格式化后的日期字符串
 */
std::wstring formatExifDate(const std::wstring& exifDate) {
    if (exifDate.length() < 10) {
        return exifDate; // Return as-is if too short
    }

    std::wstring result = exifDate;

    // Replace colons with hyphens in the date part (first 10 characters)
    for (size_t i = 0; i < result.length() && i < 10; ++i) {
        if (result[i] == L':') {
            result[i] = L'-';
        }
    }

    // Extract only the date part (YYYY-MM-DD)
    return result.substr(0, 10);
}


int wmain(int argc, wchar_t* argv[]) {
    // Validate arguments
    if (!validateArguments(argc, argv)) {
        return 1;
    }

    // Initialize COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to initialize COM library." << std::endl;
        return 1;
    }

    // RAII wrapper for COM cleanup
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

    try {
        // Get input and output paths
        fs::path inputPath(argv[1]);
        fs::path outputPath(argv[2]);

        // Ensure output path ends with filename
        fs::path outputFile = outputPath / inputPath.filename();

        std::wcout << L"Processing: " << inputPath.wstring() << std::endl;

        // Read EXIF data
        ExifReader exifReader;
        if (!exifReader.loadFromJpeg(inputPath.wstring())) {
            std::wcerr << L"Failed to load EXIF data from: " << inputPath.wstring() << std::endl;
            comCleanup();
            return 1;
        }

        // Get the date from EXIF
        auto exifDateOpt = exifReader.getDateTime();
        if (!exifDateOpt.has_value()) {
            std::wcerr << L"EXIF date not found in: " << inputPath.wstring() << std::endl;
            comCleanup();
            return 1;
        }

        std::wstring exifDate = exifDateOpt.value();
        std::wstring formattedDate = formatExifDate(exifDate);

        std::wcout << L"Found EXIF date: " << formattedDate << std::endl;

        // Add watermark to image
        if (!imageProc.addWatermarkToImage(
                inputPath.wstring(),
                formattedDate,
                outputFile.wstring())) {
            std::wcerr << L"Failed to add watermark to image." << std::endl;
            comCleanup();
            return 1;
        }

        std::wcout << L"Watermark added successfully. Output saved to: " << outputFile.wstring() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        comCleanup();
        return 1;
    } catch (...) {
        std::wcerr << L"Unknown exception occurred." << std::endl;
        comCleanup();
        return 1;
    }

    comCleanup();
    return 0;
}