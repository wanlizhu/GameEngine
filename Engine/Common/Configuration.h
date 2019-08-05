#pragma once

namespace Engine
{
    enum EConfigurationDeviceType
    {
        eDevice_D3D11 = 0,
        eDevice_D3D12 = 1,
        eDevice_OGL = 2,
    };

    enum EConfigurationMSAAType
    {
        eMSAA_Disable = 1,
        eMSAA_2 = 2,
        eMSAA_4 = 4,
        eMSAA_8 = 8,
    };

#define DECLEAR_CONFIGURATION_ITEM(name, type, value)                   \
protected:                                                              \
    type m##name = value;                                               \
public:                                                                 \
    type Get##name() const {                                            \
        return m##name;                                                 \
    }                                                                   \
    void Set##name(type _##name) {                                      \
        m##name = _##name;                                              \
    }

    class AppConfiguration
    {
    public:
        AppConfiguration() = default;

        DECLEAR_CONFIGURATION_ITEM(AppName, const char*, "Default Name")
        DECLEAR_CONFIGURATION_ITEM(Width, uint32_t, 1024)
        DECLEAR_CONFIGURATION_ITEM(Height, uint32_t, 768)
        DECLEAR_CONFIGURATION_ITEM(AppHandle, void*, nullptr)
    };

    class GraphicsConfiguration
    {
    public:
        GraphicsConfiguration() = default;
        DECLEAR_CONFIGURATION_ITEM(DeviceType, EConfigurationDeviceType, eDevice_D3D11)
        DECLEAR_CONFIGURATION_ITEM(MSAA, EConfigurationMSAAType, eMSAA_Disable)
    };

    class Configuration
    {
    public:
        Configuration() = default;

        template <typename T>
        inline T& GetConfiguration();

    protected:
        AppConfiguration mAppConfig;
        GraphicsConfiguration mGraphicsConfig;
    };

    template <typename T>
    inline T& Configuration::GetConfiguration()
    {
        static_assert(false);
    }

    template <>
    inline AppConfiguration& Configuration::GetConfiguration<AppConfiguration>()
    {
        return mAppConfig;
    }

    template <>
    inline GraphicsConfiguration& Configuration::GetConfiguration<GraphicsConfiguration>()
    {
        return mGraphicsConfig;
    }
}