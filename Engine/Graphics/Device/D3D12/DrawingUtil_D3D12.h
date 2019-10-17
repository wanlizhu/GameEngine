#pragma once

#include <d3d12.h>

namespace Engine
{
    class DrawingToD3DEnum_D3D12
    {
    public:
        DXGI_SWAP_EFFECT operator[](EDrawingSwapChainType type) const
        {
            switch (type)
            {
            case eSwapChain_Discard:
                return DXGI_SWAP_EFFECT_FLIP_DISCARD;
            case eSwapChain_Seq:
                return DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            }
            return DXGI_SWAP_EFFECT_FLIP_DISCARD;
        }

        DXGI_FORMAT operator[](EDrawingFormatType format) const
        {
            switch (format)
            {
            case eFormat_Unknown:
                return DXGI_FORMAT_UNKNOWN;
            case eFormat_R24G8_TYPELESS:
                return DXGI_FORMAT_R24G8_TYPELESS;
            case eFormat_D24S8:
            case eFormat_D24X8:
                return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case eFormat_R32_TYPELESS:
                return DXGI_FORMAT_R32_TYPELESS;
            case eFormat_D32_FLOAT:
                return DXGI_FORMAT_D32_FLOAT;
            case eFormat_R8_UNORM:
                return DXGI_FORMAT_R8_UNORM;
            case eFormat_R8_SNORM:
                return DXGI_FORMAT_R8_SNORM;
            case eFormat_R8_UINT:
                return DXGI_FORMAT_R8_UINT;
            case eFormat_R8_SINT:
                return DXGI_FORMAT_R8_SINT;
            case eFormat_R32_FLOAT:
                return DXGI_FORMAT_R32_FLOAT;
            case eFormat_R32_UINT:
                return DXGI_FORMAT_R32_UINT;
            case eFormat_R32_SINT:
                return DXGI_FORMAT_R32_SINT;
            case eFormat_R32G32_FLOAT:
                return DXGI_FORMAT_R32G32_FLOAT;
            case eFormat_R32G32_UINT:
                return DXGI_FORMAT_R32G32_UINT;
            case eFormat_R32G32_SINT:
                return DXGI_FORMAT_R32G32_SINT;
            case eFormat_R8G8B8A8_UNORM:
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case eFormat_R8G8B8A8_SNORM:
                return DXGI_FORMAT_R8G8B8A8_SNORM;
            case eFormat_R8G8B8A8_UINT:
                return DXGI_FORMAT_R8G8B8A8_UINT;
            case eFormat_R8G8B8A8_SINT:
                return DXGI_FORMAT_R8G8B8A8_SINT;
            case eFormat_R32G32B32_FLOAT:
                return DXGI_FORMAT_R32G32B32_FLOAT;
            case eFormat_R32G32B32_UINT:
                return DXGI_FORMAT_R32G32B32_UINT;
            case eFormat_R32G32B32_SINT:
                return DXGI_FORMAT_R32G32B32_SINT;
            case eFormat_R32G32B32A32_FLOAT:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case eFormat_R32G32B32A32_UINT:
                return DXGI_FORMAT_R32G32B32A32_UINT;
            case eFormat_R32G32B32A32_SINT:
                return DXGI_FORMAT_R32G32B32A32_SINT;
            }
            return DXGI_FORMAT_UNKNOWN;
        }

        D3D12_FILL_MODE operator[](EDrawingFillModeType mode) const
        {
            switch (mode)
            {
            case eFillMode_Solid:
                return D3D12_FILL_MODE_SOLID;
            case eFillMode_WireFrame:
                return D3D12_FILL_MODE_WIREFRAME;
            }
            return D3D12_FILL_MODE_SOLID;
        }

        D3D12_CULL_MODE operator[](EDrawingCullModeType mode) const
        {
            switch (mode)
            {
            case eCullMode_None:
                return D3D12_CULL_MODE_NONE;
            case eCullMode_Front:
                return D3D12_CULL_MODE_FRONT;
            case eCullMode_Back:
                return D3D12_CULL_MODE_BACK;
            }
            return D3D12_CULL_MODE_NONE;
        }

        D3D12_PRIMITIVE_TOPOLOGY operator()(EDrawingPrimitiveType prim, D3D12_PRIMITIVE_TOPOLOGY) const
        {
            switch (prim)
            {
            case ePrimitive_Undefined:
                return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
            case ePrimitive_PointList:
                return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
            case ePrimitive_LineList:
                return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
            case ePrimitive_LineStrip:
                return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
            case ePrimitive_TriangleList:
                return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case ePrimitive_TriangleStrip:
                return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            case ePrimitive_LineListAdj:
                return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
            case ePrimitive_LineStripAdj:
                return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
            case ePrimitive_TriangleListAdj:
                return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
            case ePrimitive_TriangleStripAdj:
                return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
            }
            return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        }

        D3D12_PRIMITIVE_TOPOLOGY_TYPE operator()(EDrawingPrimitiveType prim, D3D12_PRIMITIVE_TOPOLOGY_TYPE) const
        {
            switch (prim)
            {
            case ePrimitive_Undefined:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
            case ePrimitive_PointList:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            case ePrimitive_LineList:
            case ePrimitive_LineStrip:
            case ePrimitive_LineListAdj:
            case ePrimitive_LineStripAdj:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            case ePrimitive_TriangleList:
            case ePrimitive_TriangleStrip:
            case ePrimitive_TriangleListAdj:
            case ePrimitive_TriangleStripAdj:
                return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            }
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        }

        D3D12_COMMAND_LIST_TYPE operator[](EDrawingCommandListType type) const
        {
            switch (type)
            {
            case eCommandList_Direct:
                return D3D12_COMMAND_LIST_TYPE_DIRECT;
            case eCommandList_Bundle:
                return D3D12_COMMAND_LIST_TYPE_BUNDLE;
            case eCommandList_Compute:
                return D3D12_COMMAND_LIST_TYPE_COMPUTE;
            case eCommandList_Copy:
                return D3D12_COMMAND_LIST_TYPE_COPY;
            }
            return D3D12_COMMAND_LIST_TYPE_DIRECT;
        }

        D3D12_DESCRIPTOR_HEAP_TYPE operator[](EDrawingDescriptorHeapType type) const
        {
            switch (type)
            {
            case eDescriptorHeap_CBV_SRV_UVA:
                return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            case eDescriptorHeap_Sampler:
                return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            case eDescriptorHeap_RTV:
                return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            case eDescriptorHeap_DSV:
                return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            }
            return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        }
    };

    inline uint8_t D3D12FormatBytes(EDrawingFormatType format)
    {
        switch (format)
        {
        case eFormat_R32_UINT:
        case eFormat_R32_SINT:
        case eFormat_R32_FLOAT:
            return 4U;
        case eFormat_R32G32_UINT:
        case eFormat_R32G32_SINT:
        case eFormat_R32G32_FLOAT:
            return 8U;
        case eFormat_R32G32B32_UINT:
        case eFormat_R32G32B32_SINT:
        case eFormat_R32G32B32_FLOAT:
            return 12U;
        case eFormat_R32G32B32A32_UINT:
        case eFormat_R32G32B32A32_SINT:
        case eFormat_R32G32B32A32_FLOAT:
            return 16U;
        }

        return 0U;
    }

    inline const DrawingToD3DEnum_D3D12& D3D12Enum(void)
    {
        const static DrawingToD3DEnum_D3D12 d3dEnum;
        return d3dEnum;
    }

    template<typename T>
    inline auto D3D12Enum(const T& t) -> decltype(D3D12Enum()[t])
    {
        return D3D12Enum()[t];
    }

    template<typename T, typename U>
    inline auto D3D12Enum(const T& t, const U& u) -> decltype(D3D12Enum()(t, u))
    {
        return D3D12Enum()(t, u);
    }

    template<typename T>
    inline void D3D12Releaser(T* p)
    {
        p->Release();
        p = nullptr;
    }
}