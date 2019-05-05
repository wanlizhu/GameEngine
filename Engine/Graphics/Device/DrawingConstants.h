#pragma once

#include <stdint.h>

namespace Engine
{
    const uint32_t MAX_TARGETS = 8;
    const uint32_t MAX_DEPTHBUFFER = 1;
    const uint32_t MAX_RW_BUFFER = 8;
    const uint32_t MAX_VERTEX_STREAM = 32;

    const uint32_t BUFFER_COUNT = 2;

    enum EDrawingResourceType
    {
        eResource_Vertex_Buffer = 1,
        eResource_Index_Buffer,

        eResource_Vertex_Format,

        eResource_Vertex_Shader,
        eResource_Pixel_Shader,

        eResource_Effect,

        eResource_Texture,
        eResource_TexBuffer,

        eResource_Primitive,
        eResource_Varing_States,

        eResource_Blend_State,
        eResource_Depth_State,
        eResource_Raster_State,
        eResource_Sampler_State,

        eResource_Target,
        eResource_DepthBuffer,
        eResource_RWBuffer,

        // This is used for Dx12 only.
        eResource_Pipeline_State,
    };

    enum EDrawingClearFlag
    {
        eClear_Depth = 0x1,
        eClear_Stencil = 0x2,
    };

    enum EDrawingResourceFlag
    {
        eResource_Gen_Mips = 0x1,
        eResource_Cube_Map = 0x2,
        eResource_Raw_Buf = 0x4,
        eResource_Struct_Buf= 0x8,
        eResource_Draw_Indirect = 0x10,
    };

    enum EDrawingProgramType
    {
        eProgram_Shader = 1, // This is only used for effect

        eProgram_Binary,
        eProgram_String,
        eProgram_File,
    };

    enum EDrawingFormatType
    {
        eFormat_Unknown = 0,

        eFormat_D24S8,
        eFormat_D24X8,
        eFormat_D32_FLOAT,

        eFormat_R8_UNORM,
        eFormat_R8_SNORM,
        eFormat_R8_UINT,
        eFormat_R8_SINT,

        eFormat_R32_FLOAT,
        eFormat_R32_UINT,
        eFormat_R32_SINT,

        eFormat_R32G32_FLOAT,
        eFormat_R32G32_UINT,
        eFormat_R32G32_SINT,

        eFormat_R8G8B8A8_UNORM,
        eFormat_R8G8B8A8_SNORM,
        eFormat_R8G8B8A8_UINT,
        eFormat_R8G8B8A8_SINT,

        eFormat_R32G32B32_FLOAT,
        eFormat_R32G32B32_UINT,
        eFormat_R32G32B32_SINT,
        eFormat_R32G32B32A32_FLOAT,
        eFormat_R32G32B32A32_UINT,
        eFormat_R32G32B32A32_SINT,
    };

    enum EDrawingUsageType
    {
        eUsage_Default = 0,
        eUsage_Dynamic,
        eUsage_Staging,
        eUsage_Immutable,
    };

    enum EDrawingAccessType
    {
        eAccess_No_Access = 0,
        eAccess_Read,
        eAccess_Write,
        eAccess_RW,
    };

    enum EDrawingTextureType
    {
        eTexture_Unknown = 0,
        eTexture_1D,
        eTexture_1DArray,
        eTexture_2D,
        eTexture_2DArray,
        eTexture_Cube,
        eTexture_3D,
    };

    enum EDrawingBlendType
    {
        eBlend_Zero,
        eBlend_One,
        eBlend_SrcColor,
        eBlend_InvSrcColor,
        eBlend_SrcAlpha,
        eBlend_InvSrcAlpha,
        eBlend_DstAlpha,
        eBlend_InvDstAlpha,
        eBlend_DstColor,
        eBlend_InvDstColor,
        eBlend_SrcAlphaSat,
        eBlend_BlendFactor,
        eBlend_InvBlendFactor,
        eBlend_Src1Color,
        eBlend_InvSrc1Color,
        eBlend_Src1Alpha,
        eBlend_InvSrc1Alpha,
    };

    enum EDrawingBlendOpType
    {
        eBlendOp_Add,
        eBlendOp_Subtract,
        eBlendOp_RevSubtract,
        eBlendOp_Min,
        eBlendOp_Max,
    };

    enum EDrawingComparisonFuncType
    {
        eComparison_Never,
        eComparison_Less,
        eComparison_Equal,
        eComparison_LessEqual,
        eComparison_Greater,
        eComparison_NotEqual,
        eComparison_GreaterEqual,
        eComparison_Always,
    };

    enum EDrawingStencilOpType
    {
        eStencilOp_Keep,
        eStencilOp_Zero,
        eStencilOp_Replace,
        eStencilOp_IncrSat,
        eStencilOp_DecrSat,
        eStencilOp_Invert,
        eStencilOp_Incr,
        eStencilOp_Decr,
    };

    enum EDrawingFillModeType
    {
        eFillMode_Solid,
        eFillMode_WireFrame,
    };

    enum EDrawingCullModeType
    {
        eCullMode_None,
        eCullMode_Front,
        eCullMode_Back,
    };

    enum EDrawingSamplerModeType
    {
        eSamplerMode_Normal,
        eSamplerMode_Compare,
        eSamplerMode_Max,
        eSamplerMode_Min,
    };

    enum EDrawingFilterModeType
    {
        eFilterMode_Point,
        eFilterMode_Linear,
        eFilterMode_Anisotropic,
    };

    enum EDrawingAddressModeType
    {
        eAddressMode_Wrap,
        eAddressMode_Mirror,
        eAddressMode_Clamp,
        eAddressMode_Border,
        eAddressMode_MirrorOnce,
    };

    enum EDrawingPrimitiveType
    {
        ePrimitive_Undefined,
        ePrimitive_PointList,
        ePrimitive_LineList,
        ePrimitive_LineStrip,
        ePrimitive_TriangleList,
        ePrimitive_TriangleStrip,
        ePrimitive_LineListAdj,
        ePrimitive_LineStripAdj,
        ePrimitive_TriangleListAdj,
        ePrimitive_TriangleStripAdj,
        ePrimitive_ControlPointPatchList1,
        ePrimitive_ControlPointPatchList2,
        ePrimitive_ControlPointPatchList3,
        ePrimitive_ControlPointPatchList4,
        ePrimitive_ControlPointPatchList5,
        ePrimitive_ControlPointPatchList6,
        ePrimitive_ControlPointPatchList7,
        ePrimitive_ControlPointPatchList8,
        ePrimitive_ControlPointPatchList9,
        ePrimitive_ControlPointPatchList10,
        ePrimitive_ControlPointPatchList11,
        ePrimitive_ControlPointPatchList12,
        ePrimitive_ControlPointPatchList13,
        ePrimitive_ControlPointPatchList14,
        ePrimitive_ControlPointPatchList15,
        ePrimitive_ControlPointPatchList16,
        ePrimitive_ControlPointPatchList17,
        ePrimitive_ControlPointPatchList18,
        ePrimitive_ControlPointPatchList19,
        ePrimitive_ControlPointPatchList20,
        ePrimitive_ControlPointPatchList21,
        ePrimitive_ControlPointPatchList22,
        ePrimitive_ControlPointPatchList23,
        ePrimitive_ControlPointPatchList24,
        ePrimitive_ControlPointPatchList25,
        ePrimitive_ControlPointPatchList26,
        ePrimitive_ControlPointPatchList27,
        ePrimitive_ControlPointPatchList28,
        ePrimitive_ControlPointPatchList29,
        ePrimitive_ControlPointPatchList30,
        ePrimitive_ControlPointPatchList31,
        ePrimitive_ControlPointPatchList32,
    };

    enum EDrawingSwapChainType
    {
        eSwapChain_Discard,
        eSwapChain_Seq,
    };

    enum EDrawingCommandListType
    {
        eCommandList_Direct,
        eCommandList_Bundle,
        eCommandList_Compute,
        eCommandList_Copy,
        eCommandList_Count,
    };
}