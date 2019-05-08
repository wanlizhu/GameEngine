#pragma once

#include "Component.h"
#include "Vec4.h"

namespace Engine
{
    enum EProjectionType
    {
        eProjection_Perspective,
        eProjection_Orthographic,
    };

    enum EClearType
    {
        eClear_Solid,
        eClear_Skybox,
    };

    class CameraComponent : public ComponentBase<CameraComponent>
    {
    public:
        CameraComponent() : ComponentBase<CameraComponent>() {}
        CameraComponent(ERendererType rendererType,
                        EProjectionType projType,
                        EClearType clearType,
                        Vec4<float>& color,
                        float fov,
                        float near,
                        float far);

        virtual ~CameraComponent() = default;

        ERendererType GetRendererType() const;
        void SetRendererType(ERendererType type);

        EProjectionType GetProjectionType() const;
        void SetProjectionType(EProjectionType type);

        EClearType GetClearType() const;
        void SetClearType(EClearType type);

        Vec4<float> GetBackground() const;
        void SetBackground(Vec4<float>& color);

        float GetFov() const;
        void SetFov(float fov);

        float GetClippingNear() const;
        void SetClippingNear(float near);

        float GetClippingFar() const;
        void SetClippingFar(float far);

    private:
        ERendererType m_rendererType = eRenderer_Forward;
        EProjectionType m_projType = eProjection_Perspective;
        EClearType m_clearType = eClear_Solid;
        Vec4<float> m_background;

        float m_fov = 60.0f;
        float m_clippingNear = 0.3f;
        float m_clippingFar = 1000.0f;
    };
}