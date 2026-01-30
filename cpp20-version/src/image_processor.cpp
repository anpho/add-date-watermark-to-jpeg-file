#include "image_processor.h"
#include <gdiplus.h>
#include <iostream>
#include <algorithm>
#include <cmath>

ImageProcessor::ImageProcessor() : m_gdiplusToken(0) {
    m_gdiplusStartupInput.GdiplusVersion = 1;
    m_gdiplusStartupInput.DebugEventCallback = nullptr;
    m_gdiplusStartupInput.SuppressBackgroundThread = FALSE;
    m_gdiplusStartupInput.SuppressExternalCodecs = FALSE;
}

ImageProcessor::~ImageProcessor() {
    if (m_gdiplusToken != 0) {
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
    }
}

bool ImageProcessor::initialize() {
    Gdiplus::Status status = Gdiplus::GdiplusStartup(
        &m_gdiplusToken,
        &m_gdiplusStartupInput,
        nullptr
    );

    return status == Gdiplus::Ok;
}

bool ImageProcessor::addWatermarkToImage(
    const std::wstring& inputPath,
    const std::wstring& watermarkText,
    const std::wstring& outputPath
) {
    using namespace Gdiplus;

    // Load the source image
    Image* pSourceImage = Image::FromFile(inputPath.c_str());
    if (!pSourceImage || pSourceImage->GetLastStatus() != Ok) {
        std::wcerr << L"Failed to load image: " << inputPath << std::endl;
        delete pSourceImage;
        return false;
    }

    // Get image dimensions
    UINT width = pSourceImage->GetWidth();
    UINT height = pSourceImage->GetHeight();

    // Create graphics from image
    Bitmap* pBitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics* pGraphics = Graphics::FromImage(pBitmap);

    // Set high quality rendering
    pGraphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
    pGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
    pGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);

    // Draw the original image onto the new bitmap
    pGraphics->DrawImage(pSourceImage, 0, 0, width, height);

    // Calculate font size based on image dimensions
    int fontSize = calculateFontSize(width, height);

    // Create font
    Font font(L"Arial", static_cast<float>(fontSize), FontStyleBold, UnitPixel);

    // Create brush for text
    SolidBrush brush(Color(255, 255, 204, 0)); // Yellow color (255, 204, 0)

    // Measure text size
    RectF bounds;
    pGraphics->MeasureString(watermarkText.c_str(), -1, &font, PointF(0, 0), &bounds);

    // Position text at bottom-right corner with margin
    float textX = static_cast<float>(width) - bounds.Width - 20.0f;  // 20px margin
    float textY = static_cast<float>(height) - bounds.Height - 20.0f; // 20px margin

    // Draw the watermark text
    pGraphics->DrawString(
        watermarkText.c_str(),
        -1,
        &font,
        PointF(textX, textY),
        &brush
    );

    // Save the image with watermark
    CLSID jpegClsid;
    if (GetEncoderClsid(L"image/jpeg", &jpegClsid) < 0) {
        std::wcerr << L"Could not find JPEG encoder" << std::endl;
        delete pGraphics;
        delete pBitmap;
        delete pSourceImage;
        return false;
    }

    Gdiplus::EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].Guid = Gdiplus::EncoderQuality;
    encoderParams.Parameter[0].NumberOfValues = 1;
    encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
    ULONG quality = 90; // JPEG quality: 0-100
    encoderParams.Parameter[0].Value = &quality;

    Gdiplus::Status saveStatus = pBitmap->Save(outputPath.c_str(), &jpegClsid, &encoderParams);

    // Cleanup
    delete pGraphics;
    delete pBitmap;
    delete pSourceImage;

    return saveStatus == Ok;
}

int ImageProcessor::calculateFontSize(UINT imageWidth, UINT imageHeight) const {
    // Use the larger dimension to determine font size
    UINT maxDimension = std::max(imageWidth, imageHeight);

    // Base font size calculation - adjust as needed
    const int BASE_WIDTH = 900;
    const int BASE_FONT_SIZE = 24;

    double ratio = static_cast<double>(maxDimension) / BASE_WIDTH;
    int calculatedSize = static_cast<int>(BASE_FONT_SIZE * ratio);

    // Limit minimum and maximum font sizes
    const int MIN_FONT_SIZE = 12;
    const int MAX_FONT_SIZE = 72;

    return (calculatedSize < MIN_FONT_SIZE) ? MIN_FONT_SIZE : (calculatedSize > MAX_FONT_SIZE) ? MAX_FONT_SIZE : calculatedSize;
}

bool ImageProcessor::getImageDimensions(const std::wstring& imagePath, UINT& width, UINT& height) const {
    using namespace Gdiplus;

    Image* pImage = Image::FromFile(imagePath.c_str());
    if (!pImage || pImage->GetLastStatus() != Ok) {
        delete pImage;
        return false;
    }

    width = pImage->GetWidth();
    height = pImage->GetHeight();

    delete pImage;
    return true;
}

// Helper function to get encoder CLSID
int ImageProcessor::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    using namespace Gdiplus;

    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);

    if (size == 0) {
        return -1;
    }

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == nullptr) {
        return -1;
    }

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}