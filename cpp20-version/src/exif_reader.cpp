#include "exif_reader.h"
#include <wincodec.h>
#include <propvarutil.h>
#include <comdef.h>
#include <vector>
#include <sstream>

// Initialize COM
#include <atlbase.h>
#include <atlcom.h>

ExifReader::ExifReader() {
    initializeWIC();
}

ExifReader::~ExifReader() {
    // Resources will be automatically released by ComPtr
}

bool ExifReader::initializeWIC() {
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_factory)
    );

    return SUCCEEDED(hr);
}

bool ExifReader::loadFromJpeg(const std::wstring& filepath) {
    if (!m_factory) {
        return false;
    }

    HRESULT hr = m_factory->CreateDecoderFromFilename(
        filepath.c_str(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        &m_decoder
    );

    if (FAILED(hr)) {
        return false;
    }

    hr = m_decoder->GetFrame(0, &m_frame);
    if (FAILED(hr)) {
        return false;
    }

    // Query for metadata
    IWICMetadataQueryReader* tempQueryReader = nullptr;
    hr = m_frame->GetMetadataQueryReader(&tempQueryReader);
    if (FAILED(hr)) {
        return false;
    }

    m_metadataQuery.Attach(tempQueryReader);

    return true;
}

std::optional<std::wstring> ExifReader::getDateTime() const {
    if (!m_metadataQuery) {
        return std::nullopt;
    }

    PROPVARIANT varValue;
    PropVariantInit(&varValue);

    HRESULT hr = m_metadataQuery->GetMetadataByName(L"/app1/ifd/exif/{ushort=36867}", &varValue); // DateTimeOriginal
    if (FAILED(hr)) {
        // Try alternate tag for DateTime
        hr = m_metadataQuery->GetMetadataByName(L"/app1/ifd/{ushort=306}", &varValue);
    }

    if (SUCCEEDED(hr)) {
        std::optional<std::wstring> result = variantToString(varValue);
        PropVariantClear(&varValue);
        return result;
    }

    PropVariantClear(&varValue);
    return std::nullopt;
}

std::optional<std::wstring> ExifReader::getCameraMake() const {
    if (!m_metadataQuery) {
        return std::nullopt;
    }

    PROPVARIANT varValue;
    PropVariantInit(&varValue);

    HRESULT hr = m_metadataQuery->GetMetadataByName(L"/app1/ifd/{ushort=271}", &varValue);
    if (SUCCEEDED(hr)) {
        std::optional<std::wstring> result = variantToString(varValue);
        PropVariantClear(&varValue);
        return result;
    }

    PropVariantClear(&varValue);
    return std::nullopt;
}

std::optional<std::wstring> ExifReader::getCameraModel() const {
    if (!m_metadataQuery) {
        return std::nullopt;
    }

    PROPVARIANT varValue;
    PropVariantInit(&varValue);

    HRESULT hr = m_metadataQuery->GetMetadataByName(L"/app1/ifd/{ushort=272}", &varValue);
    if (SUCCEEDED(hr)) {
        std::optional<std::wstring> result = variantToString(varValue);
        PropVariantClear(&varValue);
        return result;
    }

    PropVariantClear(&varValue);
    return std::nullopt;
}

std::optional<std::wstring> ExifReader::variantToString(const PROPVARIANT& var) const {
    if (var.vt == VT_LPSTR) {
        // Convert ANSI string to wide string
        if (var.pszVal) {
            int len = MultiByteToWideChar(CP_ACP, 0, var.pszVal, -1, nullptr, 0);
            if (len > 0) {
                std::vector<wchar_t> buffer(len);
                MultiByteToWideChar(CP_ACP, 0, var.pszVal, -1, buffer.data(), len);
                return std::wstring(buffer.data());
            }
        }
    } else if (var.vt == VT_LPWSTR) {
        if (var.pwszVal) {
            return std::wstring(var.pwszVal);
        }
    } else if (var.vt == VT_BSTR) {
        if (var.bstrVal) {
            return std::wstring(var.bstrVal);
        }
    } else if (var.vt == VT_CF) {
        // Handle clipboard format
        if (var.pclipdata && var.pclipdata->pClipData) {
            DWORD size = var.pclipdata->cbSize - sizeof(DWORD);
            if (size > 0) {
                // Attempt to interpret as UTF-8 or ASCII
                std::vector<char> buffer(size + 1);
                memcpy(buffer.data(),
                       reinterpret_cast<const char*>(var.pclipdata->pClipData) + sizeof(DWORD),
                       size);
                buffer[size] = '\0';

                // Convert to wide string
                int len = MultiByteToWideChar(CP_UTF8, 0, buffer.data(), -1, nullptr, 0);
                if (len > 0) {
                    std::vector<wchar_t> wbuffer(len);
                    MultiByteToWideChar(CP_UTF8, 0, buffer.data(), -1, wbuffer.data(), len);
                    return std::wstring(wbuffer.data());
                }
            }
        }
    }

    return std::nullopt;
}