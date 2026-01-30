#pragma once

#include <wincodec.h>
#include <wrl/client.h>
#include <string>
#include <optional>
#include <memory>

/**
 * @brief EXIF数据读取器类，使用Windows Imaging Component (WIC)实现
 */
class ExifReader {
public:
    /**
     * @brief 构造函数
     */
    ExifReader();

    /**
     * @brief 析构函数
     */
    ~ExifReader();

    /**
     * @brief 从JPEG文件加载EXIF数据
     * @param filepath 文件路径
     * @return 是否成功加载
     */
    bool loadFromJpeg(const std::wstring& filepath);

    /**
     * @brief 获取拍摄日期时间
     * @return 如果存在则返回日期时间字符串，否则返回std::nullopt
     */
    std::optional<std::wstring> getDateTime() const;

    /**
     * @brief 获取相机制造商
     * @return 如果存在则返回制造商名称，否则返回std::nullopt
     */
    std::optional<std::wstring> getCameraMake() const;

    /**
     * @brief 获取相机型号
     * @return 如果存在则返回型号名称，否则返回std::nullopt
     */
    std::optional<std::wstring> getCameraModel() const;

private:
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_factory;
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> m_decoder;
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> m_frame;
    Microsoft::WRL::ComPtr<IWICMetadataQueryReader> m_metadataQuery;

    /**
     * @brief 初始化WIC工厂
     * @return 是否初始化成功
     */
    bool initializeWIC();

    /**
     * @brief 将PROPVARIANT转换为wstring
     * @param var 要转换的变体
     * @return 转换后的字符串，如果失败则返回std::nullopt
     */
    std::optional<std::wstring> variantToString(const PROPVARIANT& var) const;
};