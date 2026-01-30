# Add Date Watermark to JPEG Files (C++20 Version)

This is a modern C++20 implementation of a command-line tool to add DATE watermarks to JPEG files. It uses Windows Imaging Component (WIC) and GDI+ for image processing, replacing the Qt dependency of the original implementation.

## Features

- Reads EXIF data from JPEG files to extract the photo capture date
- Adds the date as a watermark to the bottom-right corner of the image
- Automatically scales the watermark font size based on image dimensions
- Uses modern C++20 features and Windows platform APIs
- Lightweight and fast processing

## Prerequisites

- Windows OS (Windows 7 or later)
- C++20 compatible compiler (Visual Studio 2019 or later recommended)
- CMake 3.20 or later
- Windows SDK

## Building

1. Clone the repository
2. Create a build directory: `mkdir build && cd build`
3. Configure with CMake: `cmake .. -G "Visual Studio 16 2019"`
4. Build the project: `cmake --build . --config Release`

## Usage

```
add-date-watermark-cpp20 <input_file> <output_directory>
```

Example:
```
add-date-watermark-cpp20 D:\photo.jpg D:\output
```

## Technical Details

### Architecture
- **EXIF Reading**: Uses Windows Imaging Component (WIC) to read EXIF metadata
- **Image Processing**: Uses GDI+ for image manipulation and text rendering
- **Memory Management**: RAII principles with smart pointers
- **Modern C++**: C++20 features like concepts, ranges, modules, etc.

### Key Improvements Over Original
- Modern C++20 codebase
- No external dependencies beyond Windows APIs
- Better resource management with RAII
- Improved error handling
- More efficient memory usage
- Cleaner, more maintainable code structure

## License

MIT License - see the LICENSE file for details.