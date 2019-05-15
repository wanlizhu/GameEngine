#pragma once

#include "Component.h"
#include "IRenderer.h"

namespace Engine
{
    class PolylineRendererComponent : public ComponentBase<PolylineRendererComponent>
    {
    public:
        PolylineRendererComponent() : ComponentBase<PolylineRendererComponent>() {}
        virtual ~PolylineRendererComponent() = default;

        struct LineSegmentGeom
        {
            float2 startPoint;
            float2 endPoint;
            uint32_t color;
            float drawZ;
        };

        LineSegmentGeom GetGeometry() const;
        void SetGeometry(const LineSegmentGeom& geom);

    private:
        LineSegmentGeom m_geom;
    };
}