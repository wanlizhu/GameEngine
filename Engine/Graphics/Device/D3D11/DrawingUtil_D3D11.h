#pragma once

#include <d3d11.h>

#include "DrawingConstants.h"

namespace Engine
{
    class DrawingToD3DEnum_D3D11
    {
    public:
        DXGI_SWAP_EFFECT operator[](EDrawingSwapChainType type) const
        {
            switch (type)
            {
            case eSwapChain_Discard:
                return DXGI_SWAP_EFFECT_DISCARD;
            case eSwapChain_Seq:
                return DXGI_SWAP_EFFECT_SEQUENTIAL;
            }
            return DXGI_SWAP_EFFECT_DISCARD;
        }

        DXGI_FORMAT operator[](EDrawingFormatType format) const
        {
            switch (format)
            {
            case eFormat_Unknown:
                return DXGI_FORMAT_UNKNOWN;
            case eFormat_D24S8:
            case eFormat_D24X8:
                return DXGI_FORMAT_D24_UNORM_S8_UINT;
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

        D3D11_USAGE operator[](EDrawingUsageType usage) const
        {
            switch (usage)
            {
            case eUsage_Default:
                return D3D11_USAGE_DEFAULT;
            case eUsage_Dynamic:
                return D3D11_USAGE_DYNAMIC;
            case eUsage_Staging:
                return D3D11_USAGE_STAGING;
            case eUsage_Immutable:
                return D3D11_USAGE_IMMUTABLE;
            }
            return D3D11_USAGE_DEFAULT;
        }

        UINT operator[](EDrawingAccessType access) const
        {
            switch (access)
            {
            case eAccess_No_Access:
                return 0;
            case eAccess_Read:
                return D3D11_CPU_ACCESS_READ;
            case eAccess_Write:
            case eAccess_Write_Discard:
            case eAccess_Write_Append:
                return D3D11_CPU_ACCESS_WRITE;
            case eAccess_RW:
                return D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            }
            return eAccess_No_Access;
        }

        D3D11_BLEND operator[](EDrawingBlendType blend) const
        {
            switch (blend)
            {
            case eBlend_Zero:
                return D3D11_BLEND_ZERO;
            case eBlend_One:
                return D3D11_BLEND_ONE;
            case eBlend_SrcColor:
                return D3D11_BLEND_SRC_COLOR;
            case eBlend_InvSrcColor:
                return D3D11_BLEND_INV_SRC_COLOR;
            case eBlend_SrcAlpha:
                return D3D11_BLEND_SRC_ALPHA;
            case eBlend_InvSrcAlpha:
                return D3D11_BLEND_INV_SRC_ALPHA;
            case eBlend_DstAlpha:
                return D3D11_BLEND_DEST_ALPHA;
            case eBlend_InvDstAlpha:
                return D3D11_BLEND_INV_DEST_ALPHA;
            case eBlend_DstColor:
                return D3D11_BLEND_DEST_COLOR;
            case eBlend_InvDstColor:
                return D3D11_BLEND_INV_DEST_COLOR;
            case eBlend_SrcAlphaSat:
                return D3D11_BLEND_SRC_ALPHA_SAT;
            case eBlend_BlendFactor:
                return D3D11_BLEND_BLEND_FACTOR;
            case eBlend_InvBlendFactor:
                return D3D11_BLEND_INV_BLEND_FACTOR;
            case eBlend_Src1Color:
                return D3D11_BLEND_SRC1_COLOR;
            case eBlend_InvSrc1Color:
                return D3D11_BLEND_INV_SRC1_COLOR;
            case eBlend_Src1Alpha:
                return D3D11_BLEND_SRC1_ALPHA;
            case eBlend_InvSrc1Alpha:
                return D3D11_BLEND_INV_SRC1_ALPHA;
            }
            return D3D11_BLEND_ZERO;
        }

        D3D11_BLEND_OP operator[](EDrawingBlendOpType op) const
        {
            switch (op)
            {
            case eBlendOp_Add:
                return D3D11_BLEND_OP_ADD;
            case eBlendOp_Subtract:
                return D3D11_BLEND_OP_SUBTRACT;
            case eBlendOp_RevSubtract:
                return D3D11_BLEND_OP_REV_SUBTRACT;
            case eBlendOp_Min:
                return D3D11_BLEND_OP_MIN;
            case eBlendOp_Max:
                return D3D11_BLEND_OP_MAX;
            }
            return D3D11_BLEND_OP_ADD;
        }

        D3D11_COMPARISON_FUNC operator[](EDrawingComparisonFuncType func) const
        {
            switch (func)
            {
            case eComparison_Never:
                return D3D11_COMPARISON_NEVER;
            case eComparison_Less:
                return D3D11_COMPARISON_LESS;
            case eComparison_Equal:
                return D3D11_COMPARISON_EQUAL;
            case eComparison_LessEqual:
                return D3D11_COMPARISON_LESS_EQUAL;
            case eComparison_Greater:
                return D3D11_COMPARISON_GREATER;
            case eComparison_NotEqual:
                return D3D11_COMPARISON_NOT_EQUAL;
            case eComparison_GreaterEqual:
                return D3D11_COMPARISON_GREATER_EQUAL;
            case eComparison_Always:
                return D3D11_COMPARISON_ALWAYS;
            }
            return D3D11_COMPARISON_LESS_EQUAL;
        }

        D3D11_STENCIL_OP operator[](EDrawingStencilOpType op) const
        {
            switch (op)
            {
            case eStencilOp_Keep:
                return D3D11_STENCIL_OP_KEEP;
            case eStencilOp_Zero:
                return D3D11_STENCIL_OP_ZERO;
            case eStencilOp_Replace:
                return D3D11_STENCIL_OP_REPLACE;
            case eStencilOp_IncrSat:
                return D3D11_STENCIL_OP_INCR_SAT;
            case eStencilOp_DecrSat:
                return D3D11_STENCIL_OP_DECR_SAT;
            case eStencilOp_Invert:
                return D3D11_STENCIL_OP_INVERT;
            case eStencilOp_Incr:
                return D3D11_STENCIL_OP_INCR;
            case eStencilOp_Decr:
                return D3D11_STENCIL_OP_DECR;
            }
            return D3D11_STENCIL_OP_KEEP;
        }

        D3D11_FILL_MODE operator[](EDrawingFillModeType mode) const
        {
            switch (mode)
            {
            case eFillMode_Solid:
                return D3D11_FILL_SOLID;
            case eFillMode_WireFrame:
                return D3D11_FILL_WIREFRAME;
            }
            return D3D11_FILL_SOLID;
        }

        D3D11_CULL_MODE operator[](EDrawingCullModeType mode) const
        {
            switch (mode)
            {
            case eCullMode_None:
                return D3D11_CULL_NONE;
            case eCullMode_Front:
                return D3D11_CULL_FRONT;
            case eCullMode_Back:
                return D3D11_CULL_BACK;
            }
            return D3D11_CULL_NONE;
        }

        D3D11_TEXTURE_ADDRESS_MODE operator[](EDrawingAddressModeType mode) const
        {
            switch (mode)
            {
            case eAddressMode_Wrap:
                return D3D11_TEXTURE_ADDRESS_WRAP;
            case eAddressMode_Mirror:
                return D3D11_TEXTURE_ADDRESS_MIRROR;
            case eAddressMode_Clamp:
                return D3D11_TEXTURE_ADDRESS_CLAMP;
            case eAddressMode_Border:
                return D3D11_TEXTURE_ADDRESS_BORDER;
            case eAddressMode_MirrorOnce:
                return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
            }
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        }

        D3D11_PRIMITIVE_TOPOLOGY operator[](EDrawingPrimitiveType prim) const
        {
            switch (prim)
            {
            case ePrimitive_Undefined:
                return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
            case ePrimitive_PointList:
                return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            case ePrimitive_LineList:
                return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            case ePrimitive_LineStrip:
                return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
            case ePrimitive_TriangleList:
                return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case ePrimitive_TriangleStrip:
                return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            case ePrimitive_LineListAdj:
                return D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
            case ePrimitive_LineStripAdj:
                return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
            case ePrimitive_TriangleListAdj:
                return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
            case ePrimitive_TriangleStripAdj:
                return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
            }
            return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        }

        D3D11_FILTER operator()(EDrawingSamplerModeType mode, EDrawingFilterModeType min, EDrawingFilterModeType mag, EDrawingFilterModeType mip) const
        {
            return D3D11_FILTER_MIN_MAG_MIP_POINT;
        }
    };

    inline const char* D3D11HLSLFormatToString(EDrawingFormatType format)
    {
        switch (format)
        {
        case eFormat_R32_UINT:
            return "uint";
        case eFormat_R32_SINT:
            return "int";
        case eFormat_R32_FLOAT:
            return "float";
        case eFormat_R32G32_UINT:
            return "uint2";
        case eFormat_R32G32_SINT:
            return "int2";
        case eFormat_R32G32_FLOAT:
            return "float2";
        case eFormat_R32G32B32_UINT:
            return "uint3";
        case eFormat_R32G32B32_SINT:
            return "int3";
        case eFormat_R32G32B32_FLOAT:
            return "float3";
        case eFormat_R32G32B32A32_UINT:
            return "uint4";
        case eFormat_R32G32B32A32_SINT:
            return "int4";
        case eFormat_R32G32B32A32_FLOAT:
            return "float4";
        }
        return "float4";
    }

    inline uint8_t D3D11FormatBytes(EDrawingFormatType format)
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

    inline UINT D3D11ResourceMiscFlag(uint32_t flags)
    {
        UINT d3d11MiscFlags = 0;
        if ((d3d11MiscFlags & eResource_Gen_Mips) != 0)
            d3d11MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

        if ((d3d11MiscFlags & eResource_Cube_Map) != 0)
            d3d11MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

        if ((d3d11MiscFlags & eResource_Raw_Buf) != 0)
            d3d11MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

        if ((d3d11MiscFlags & eResource_Struct_Buf) != 0)
            d3d11MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

        if ((d3d11MiscFlags & eResource_Draw_Indirect) != 0)
            d3d11MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

        return d3d11MiscFlags;
    }

    inline D3D11_MAP D3D11ResourceMapType(EDrawingAccessType access)
    {
        switch (access)
        {
        case eAccess_Read:
            return D3D11_MAP_READ;

        case eAccess_Write:
            return D3D11_MAP_WRITE;

        case eAccess_RW:
            return D3D11_MAP_READ_WRITE;

        case eAccess_Write_Append:
            return D3D11_MAP_WRITE_NO_OVERWRITE;

        case eAccess_No_Access:
        case eAccess_Write_Discard:
        default:
            return D3D11_MAP_WRITE_DISCARD;
        }
    }

    inline const DrawingToD3DEnum_D3D11& D3D11Enum(void)
    {
        const static DrawingToD3DEnum_D3D11 d3dEnum;
        return d3dEnum;
    }

    template<typename T>
    inline auto D3D11Enum(const T& t) -> decltype(D3D11Enum()[t])
    {
        return D3D11Enum()[t];
    }

    template<typename T, typename U>
    inline auto D3D11Enum(const T& t, const U& u1, const U& u2, const U& u3) -> decltype(D3D11Enum()(t, u1, u2, u3))
    {
        return D3D11Enum()(t, u1, u2, u3);
    }

    template<typename T>
    inline void D3D11Releaser(T* p)
    {
        p->Release();
    }
}