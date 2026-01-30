#pragma once

#include <windows.h>
#undef min
#undef max
#include <gdiplus.h>
#include <string>
#include <memory>
#include <filesystem>
#include <algorithm>

// 自定义 clamp 函数声明
template<class T>
constexpr const T& clamp_func(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

/**
 * @brief 图像处理器类，负责图像操作和水印添加
 */
class ImageProcessor {
public:
    /**
     * @brief 构造函数
     */
    ImageProcessor();

    /**
     * @brief 析构函数
     */
    ~ImageProcessor();

    /**
     * @brief 初始化GDI+
     */
    bool initialize();

    /**
     * @brief 向图像添加水印
     * @param inputPath 输入图像路径
     * @param watermarkText 水印文本
     * @param outputPath 输出图像路径
     * @return 是否成功处理
     */
    bool addWatermarkToImage(
        const std::wstring& inputPath,
        const std::wstring& watermarkText,
        const std::wstring& outputPath
    );

private:
    ULONG_PTR m_gdiplusToken;
    Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;

    /**
     * @brief 计算合适的字体大小
     * @param imageWidth 图像宽度
     * @param imageHeight 图像高度
     * @return 计算出的字体大小
     */
    int calculateFontSize(UINT imageWidth, UINT imageHeight) const;

    /**
     * @brief 获取图像尺寸
     * @param imagePath 图像路径
     * @param width 宽度输出
     * @param height 高度输出
     * @return 是否成功获取
     */
    bool getImageDimensions(const std::wstring& imagePath, UINT& width, UINT& height) const;

    /**
     * @brief 获取编码器CLSID
     * @param format 格式字符串
     * @param pClsid CLSID输出
     * @return 成功返回索引，失败返回-1
     */
    int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
};