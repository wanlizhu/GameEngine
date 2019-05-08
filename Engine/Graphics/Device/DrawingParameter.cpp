#include <assert.h>

#include "DrawingParameter.h"
#include "DrawingRawResource.h"

using namespace Engine;

DrawingParameter::DrawingParameter() :
    m_pName(nullptr),
    m_pSemantic(nullptr),
    m_pValue(nullptr),
    m_size(0),
    m_type(0)
{
}

DrawingParameter::DrawingParameter(const std::shared_ptr<std::string> pName, uint32_t type, void* pInitVal, const std::shared_ptr<std::string> pSemantic) :
    m_pName(pName),
    m_pSemantic(pSemantic)
{
    CreateParameter(type, pInitVal);
}

DrawingParameter::~DrawingParameter()
{
}

const uint32_t DrawingParameter::BasicTypeSize[eBasic_Count] = 
{
    sizeof(bool),
    sizeof(uint32_t),
    sizeof(int32_t),
    sizeof(float),
    sizeof(double),
};

const uint32_t DrawingParameter::GetBitsValue(uint32_t val, uint32_t bits, uint32_t offset)
{
    return ((val >> offset) & ((0x1 << bits) - 1));
}

const uint32_t DrawingParameter::GetObjectType(uint32_t type)
{
    return GetBitsValue(type, Object_Type_Bits, Object_Type_Offset);
}

const uint32_t DrawingParameter::GetDataSetType(uint32_t type)
{
    return GetBitsValue(type, DataSet_Type_Bits, DataSet_Type_Offset);
}

const uint32_t DrawingParameter::GetBasicType(uint32_t type)
{
    return GetBitsValue(type, Basic_Type_Bits, Basic_Type_Offset);
}

const uint32_t DrawingParameter::GetArraySize(uint32_t type)
{
    return GetBitsValue(type, Array_Size_Bits, Array_Size_Offset);
}

const uint32_t DrawingParameter::GetRowSize(uint32_t type)
{
    return GetBitsValue(type, Row_Size_Bits, Row_Size_Offset);
}

const uint32_t DrawingParameter::GetColSize(uint32_t type)
{
    return GetBitsValue(type, Col_Size_Bits, Col_Size_Offset);
}

const uint32_t DrawingParameter::GetStructSize(uint32_t type)
{
    return GetBitsValue(type, Struct_Size_Bits, Struct_Size_Offset);
}

std::shared_ptr<std::string> DrawingParameter::GetName() const
{
    return m_pName;
}

void DrawingParameter::SetName(std::shared_ptr<std::string> pName)
{
    m_pName = pName;
}

bool DrawingParameter::IsDirty() const
{
    return m_bDirty;
}

void DrawingParameter::SetDirty(bool bDirty)
{
    m_bDirty = bDirty;
}

std::shared_ptr<std::string> DrawingParameter::GetSemantic() const
{
    return m_pSemantic;
}

void DrawingParameter::SetSemantic(std::shared_ptr<std::string> pSemantic)
{
    m_pSemantic = pSemantic;
}

uint32_t DrawingParameter::GetSize() const
{
    return m_size;
}

void DrawingParameter::SetSize(uint32_t size)
{
    m_size = size;
}

uint32_t DrawingParameter::GetType() const
{
    return m_type;
}

void DrawingParameter::SetType(uint32_t type)
{
    m_type = type;
}

const void* const DrawingParameter::GetValuePtr() const
{
    return m_pValue;
}

uint32_t DrawingParameter::GetValueSize() const
{
    return m_size;
}

void DrawingParameter::SetValue(const void* pInitVal, uint32_t size)
{
    assert(size == m_size);

    if (pInitVal != nullptr)
    {
        if (memcmp(m_pValue, pInitVal, size) != 0)
        {
            memcpy(m_pValue, pInitVal, size);
            m_bDirty = true;
        }
    }
    else
    {
        static const unsigned int zeroMem[4096] = { 0 };
        if (memcmp(m_pValue, zeroMem, size) != 0)
        {
            memset(m_pValue, 0, size);
            m_bDirty = true;
        }
    }
}

uint32_t DrawingParameter::GetObjectType() const
{
    return GetObjectType(m_type);
}

uint32_t DrawingParameter::GetDataSetType() const
{
    return GetDataSetType(m_type);
}

uint32_t DrawingParameter::GetBasicType() const
{
    return GetBasicType(m_type);
}

uint32_t DrawingParameter::GetArraySize() const
{
    return GetArraySize(m_type);
}

uint32_t DrawingParameter::GetRowSize() const
{
    return GetRowSize(m_type);
}

uint32_t DrawingParameter::GetColSize() const
{
    return GetColSize(m_type);
}

uint32_t DrawingParameter::GetStructSize() const
{
    return GetStructSize(m_type);
}

uint32_t DrawingParameter::GetVectorSize() const
{
    return GetRowSize(m_type);
}

const bool& DrawingParameter::AsBool() const
{
    return AsScalar<bool, eBasic_Bool>();
}

void DrawingParameter::AsBool(const bool& val)
{
    return AsScalar<bool, eBasic_Bool>(val);
}

const bool2& DrawingParameter::AsBool2() const
{
    return AsVector<bool2, eBasic_Bool, 2>();
}

void DrawingParameter::AsBool2(const bool2& val)
{
    AsVector<bool2, eBasic_Bool, 2>(val);
}

const bool3& DrawingParameter::AsBool3() const
{
    return AsVector<bool3, eBasic_Bool, 3>();
}

void DrawingParameter::AsBool3(const bool3& val)
{
    AsVector<bool3, eBasic_Bool, 3>(val);
}

const bool4& DrawingParameter::AsBool4() const
{
    return AsVector<bool4, eBasic_Bool, 4>();
}

void DrawingParameter::AsBool4(const bool4& val)
{
    AsVector<bool4, eBasic_Bool, 4>(val);
}

const uint32_t& DrawingParameter::AsUInt() const
{
    return AsScalar<uint32_t, eBasic_UInt32>();
}

void DrawingParameter::AsUInt(const uint32_t& val)
{
    AsScalar<uint32_t, eBasic_UInt32>(val);
}

const uint2& DrawingParameter::AsUInt2() const
{
    return AsVector<uint2, eBasic_UInt32, 2>();
}

void DrawingParameter::AsUInt2(const uint2& val)
{
    AsVector<uint2, eBasic_UInt32, 2>(val);
}

const uint3& DrawingParameter::AsUInt3() const
{
    return AsVector<uint3, eBasic_UInt32, 3>();
}

void DrawingParameter::AsUInt3(const uint3& val)
{
    AsVector<uint3, eBasic_UInt32, 3>(val);
}

const uint4& DrawingParameter::AsUInt4() const
{
    return AsVector<uint4, eBasic_UInt32, 4>();
}

void DrawingParameter::AsUInt4(const uint4& val)
{
    AsVector<uint4, eBasic_UInt32, 4>(val);
}

const int32_t& DrawingParameter::AsInt() const
{
    return AsScalar<int32_t, eBasic_Int32>();
}

void DrawingParameter::AsInt(const int32_t& val)
{
    AsScalar<int32_t, eBasic_Int32>(val);
}

const int2& DrawingParameter::AsInt2() const
{
    return AsVector<int2, eBasic_Int32, 2>();
}

void DrawingParameter::AsInt2(const int2& val)
{
    AsVector<int2, eBasic_Int32, 2>(val);
}

const int3& DrawingParameter::AsInt3() const
{
    return AsVector<int3, eBasic_Int32, 3>();
}

void DrawingParameter::AsInt3(const int3& val)
{
    AsVector<int3, eBasic_Int32, 3>(val);
}

const int4& DrawingParameter::AsInt4() const
{
    return AsVector<int4, eBasic_Int32, 4>();
}

void DrawingParameter::AsInt4(const int4& val)
{
    AsVector<int4, eBasic_Int32, 4>(val);
}

const float& DrawingParameter::AsFloat() const
{
    return AsScalar<float, eBasic_FP32>();
}

void DrawingParameter::AsFloat(const float& val)
{
    AsScalar<float, eBasic_FP32>(val);
}

const float2& DrawingParameter::AsFloat2() const
{
    return AsVector<float2, eBasic_FP32, 2>();
}

void DrawingParameter::AsFloat2(const float2& val)
{
    AsVector<float2, eBasic_FP32, 2>(val);
}

const float3& DrawingParameter::AsFloat3() const
{
    return AsVector<float3, eBasic_FP32, 3>();
}

void DrawingParameter::AsFloat3(const float3& val)
{
    AsVector<float3, eBasic_FP32, 3>(val);
}

const float4& DrawingParameter::AsFloat4() const
{
    return AsVector<float4, eBasic_FP32, 4>();
}

void DrawingParameter::AsFloat4(const float4& val)
{
    AsVector<float4, eBasic_FP32, 4>(val);
}
    
const float2x2& DrawingParameter::AsFloat2x2() const
{
    return AsMatrix<float2x2, eBasic_FP32, 2, 2>();
}

void DrawingParameter::AsFloat2x2(const float2x2& val)
{
    AsMatrix<float2x2, eBasic_FP32, 2, 2>(val);
}

const float3x3& DrawingParameter::AsFloat3x3() const
{
    return AsMatrix<float3x3, eBasic_FP32, 3, 3>();
}

void DrawingParameter::AsFloat3x3(const float3x3& val)
{
    AsMatrix<float3x3, eBasic_FP32, 3, 3>(val);
}

const float4x4& DrawingParameter::AsFloat4x4() const
{
    return AsMatrix<float4x4, eBasic_FP32, 4, 4>();
}

void DrawingParameter::AsFloat4x4(const float4x4& val)
{
    AsMatrix<float4x4, eBasic_FP32, 4, 4>(val);
}

const double& DrawingParameter::AsDouble() const
{
    return AsScalar<double, eBasic_FP64>();
}

void DrawingParameter::AsDouble(const double& val)
{
    AsScalar<double, eBasic_FP64>(val);
}

const double2& DrawingParameter::AsDouble2() const
{
    return AsVector<double2, eBasic_FP64, 2>();
}

void DrawingParameter::AsDouble2(const double2& val)
{
    AsVector<double2, eBasic_FP64, 2>(val);
}

const double3& DrawingParameter::AsDouble3() const
{
    return AsVector<double3, eBasic_FP64, 3>();
}

void DrawingParameter::AsDouble3(const double3& val)
{
    AsVector<double3, eBasic_FP64, 3>(val);
}

const double4& DrawingParameter::AsDouble4() const
{
    return AsVector<double4, eBasic_FP64, 4>(); 
}

void DrawingParameter::AsDouble4(const double4& val)
{
    AsVector<double4, eBasic_FP64, 4>(val);
}

const double2x2& DrawingParameter::AsDouble2x2() const
{
    return AsMatrix<double2x2, eBasic_FP64, 2, 2>();
}

void DrawingParameter::AsDouble2x2(const double2x2& val)
{
    AsMatrix<double2x2, eBasic_FP64, 2, 2>(val);
}

const double3x3& DrawingParameter::AsDouble3x3() const
{
    return AsMatrix<double3x3, eBasic_FP64, 3, 3>();
}

void DrawingParameter::AsDouble3x3(const double3x3& val)
{
    AsMatrix<double3x3, eBasic_FP64, 3, 3>(val);
}

const double4x4& DrawingParameter::AsDouble4x4() const
{
    return AsMatrix<double4x4, eBasic_FP64, 4, 4>();
}

void DrawingParameter::AsDouble4x4(const double4x4& val)
{
    AsMatrix<double4x4, eBasic_FP64, 4, 4>(val);
}

const DrawingRawTexture* DrawingParameter::AsTexture() const
{
    assert(GetObjectType() == eObject_Texture);

    return AsValue<const DrawingRawTexture*>();
}

void DrawingParameter::AsTexture(const DrawingRawTexture* pTexture)
{
    assert(GetObjectType() == eObject_Texture);

    AsValue<const DrawingRawTexture*>(pTexture);
}

const DrawingRawTexBuffer* DrawingParameter::AsBuffer() const
{
    assert(GetObjectType() == eObject_Buffer);

    return AsValue<const DrawingRawTexBuffer*>();
}

void DrawingParameter::AsBuffer(const DrawingRawTexBuffer* pBuffer)
{
    assert(GetObjectType() == eObject_Buffer);

    AsValue<const DrawingRawTexBuffer*>(pBuffer);
}

const DrawingRawRWBuffer* DrawingParameter::AsRWBuffer() const
{
    assert(GetObjectType() == eObject_RWBuffer);

    return AsValue<const DrawingRawRWBuffer*>();
}

void DrawingParameter::AsRWBuffer(const DrawingRawRWBuffer* pRWBuffer)
{
    assert(GetObjectType() == eObject_RWBuffer);

    AsValue<const DrawingRawRWBuffer*>(pRWBuffer);
}

const DrawingRawTexBuffer* DrawingParameter::AsTexBuffer() const
{
    assert(GetObjectType() == eObject_TexBuffer);

    return AsValue<const DrawingRawTexBuffer*>();
}

void DrawingParameter::AsTexBuffer(const DrawingRawTexBuffer* pTexBuffer)
{
    assert(GetObjectType() == eObject_TexBuffer);

    AsValue<const DrawingRawTexBuffer*>(pTexBuffer);
}

const DrawingRawSamplerState* DrawingParameter::AsSampler() const
{
    assert(GetObjectType() == eObject_Sampler);

    return AsValue<const DrawingRawSamplerState*>();
}

void DrawingParameter::AsSampler(const DrawingRawSamplerState* pState)
{
    assert(GetObjectType() == eObject_Sampler);

    AsValue<const DrawingRawSamplerState*>(pState);
}

const bool* DrawingParameter::AsBoolArray(uint32_t& array_size) const
{
    return AsScalarArray<bool, eBasic_Bool>(array_size);
}

void DrawingParameter::AsBoolArray(const bool* val, uint32_t array_size)
{
    AsScalarArray<bool, eBasic_Bool>(val, array_size);
}

const bool2* DrawingParameter::AsBool2Array(uint32_t& array_size) const
{
    return AsVectorArray<bool2, eBasic_Bool, 2>(array_size);
}

void DrawingParameter::AsBool2Array(const bool2* val, uint32_t array_size)
{
    AsVectorArray<bool2, eBasic_Bool, 2>(val, array_size);
}

const bool3* DrawingParameter::AsBool3Array(uint32_t& array_size) const
{
    return AsVectorArray<bool3, eBasic_Bool, 3>(array_size);
}

void DrawingParameter::AsBool3Array(const bool3* val, uint32_t array_size)
{
    AsVectorArray<bool3, eBasic_Bool, 3>(val, array_size);
}

const bool4* DrawingParameter::AsBool4Array(uint32_t& array_size) const
{
    return AsVectorArray<bool4, eBasic_Bool, 4>(array_size);
}

void DrawingParameter::AsBool4Array(const bool4* val, uint32_t array_size)
{
    AsVectorArray<bool4, eBasic_Bool, 4>(val, array_size);
}

const uint32_t* DrawingParameter::AsUIntArray(uint32_t& array_size) const
{
    return AsScalarArray<uint32_t, eBasic_UInt32>(array_size);
}

void DrawingParameter::AsUIntArray(const uint32_t* val, uint32_t array_size)
{
    AsScalarArray<uint32_t, eBasic_UInt32>(val, array_size);
}

const uint2* DrawingParameter::AsUInt2Array(uint32_t& array_size) const
{
    return AsVectorArray<uint2, eBasic_UInt32, 2>(array_size);
}

void DrawingParameter::AsUInt2Array(const uint2* val, uint32_t array_size)
{
    AsVectorArray<uint2, eBasic_UInt32, 2>(val, array_size);
}

const uint3* DrawingParameter::AsUInt3Array(uint32_t& array_size) const
{
    return AsVectorArray<uint3, eBasic_UInt32, 3>(array_size);
}

void DrawingParameter::AsUInt3Array(const uint3* val, uint32_t array_size)
{
    AsVectorArray<uint3, eBasic_UInt32, 3>(val, array_size);
}

const uint4* DrawingParameter::AsUInt4Array(uint32_t& array_size) const
{
    return AsVectorArray<uint4, eBasic_UInt32, 4>(array_size);
}

void DrawingParameter::AsUInt4Array(const uint4* val, uint32_t array_size)
{
    AsVectorArray<uint4, eBasic_UInt32, 4>(val, array_size);
}

const int32_t* DrawingParameter::AsIntArray(uint32_t& array_size) const
{
    return AsScalarArray<int32_t, eBasic_Int32>(array_size);
}

void DrawingParameter::AsIntArray(const int32_t* val, uint32_t array_size)
{
    AsScalarArray<int32_t, eBasic_Int32>(val, array_size);
}

const int2* DrawingParameter::AsInt2Array(uint32_t& array_size) const
{
    return AsVectorArray<int2, eBasic_Int32, 2>(array_size);
}

void DrawingParameter::AsInt2Array(const int2* val, uint32_t array_size)
{
    AsVectorArray<int2, eBasic_Int32, 2>(val, array_size);
}

const int3* DrawingParameter::AsInt3Array(uint32_t& array_size) const
{
    return AsVectorArray<int3, eBasic_Int32, 3>(array_size);
}

void DrawingParameter::AsInt3Array(const int3* val, uint32_t array_size)
{
    AsVectorArray<int3, eBasic_Int32, 3>(val, array_size);
}

const int4* DrawingParameter::AsInt4Array(uint32_t& array_size) const
{
    return AsVectorArray<int4, eBasic_Int32, 4>(array_size);
}

void DrawingParameter::AsInt4Array(const int4* val, uint32_t array_size)
{
    AsVectorArray<int4, eBasic_Int32, 4>(val, array_size);
}

const float* DrawingParameter::AsFloatArray(uint32_t& array_size) const
{
    return AsScalarArray<float, eBasic_FP32>(array_size);
}

void DrawingParameter::AsFloatArray(const float* val, uint32_t array_size)
{
    AsScalarArray<float, eBasic_FP32>(val, array_size);
}

const float2* DrawingParameter::AsFloat2Array(uint32_t& array_size) const
{
    return AsVectorArray<float2, eBasic_FP32, 2>(array_size);
}

void DrawingParameter::AsFloat2Array(const float2* val, uint32_t array_size)
{
    AsVectorArray<float2, eBasic_FP32, 2>(val, array_size);
}

const float3* DrawingParameter::AsFloat3Array(uint32_t& array_size) const
{
    return AsVectorArray<float3, eBasic_FP32, 3>(array_size);
}

void DrawingParameter::AsFloat3Array(const float3* val, uint32_t array_size)
{
    AsVectorArray<float3, eBasic_FP32, 3>(val, array_size);
}

const float4* DrawingParameter::AsFloat4Array(uint32_t& array_size) const
{
    return AsVectorArray<float4, eBasic_FP32, 4>(array_size);
}

void DrawingParameter::AsFloat4Array(const float4* val, uint32_t array_size)
{
    AsVectorArray<float4, eBasic_FP32, 4>(val, array_size);
}

const float2x2* DrawingParameter::AsFloat2x2Array(uint32_t& array_size) const
{
    return AsMatrixArray<float2x2, eBasic_FP32, 2, 2>(array_size);
}

void DrawingParameter::AsFloat2x2Array(const float2x2* val, uint32_t array_size)
{
    AsMatrixArray<float2x2, eBasic_FP32, 2, 2>(val, array_size);
}

const float3x3* DrawingParameter::AsFloat3x3Array(uint32_t& array_size) const
{
    return AsMatrixArray<float3x3, eBasic_FP32, 3, 3>(array_size);
}

void DrawingParameter::AsFloat3x3Array(const float3x3* val, uint32_t array_size)
{
    AsMatrixArray<float3x3, eBasic_FP32, 3, 3>(val, array_size);
}

const float4x4* DrawingParameter::AsFloat4x4Array(uint32_t& array_size) const
{
    return AsMatrixArray<float4x4, eBasic_FP32, 4, 4>(array_size);
}

void DrawingParameter::AsFloat4x4Array(const float4x4* val, uint32_t array_size)
{
    AsMatrixArray<float4x4, eBasic_FP32, 4, 4>(val, array_size);
}

const double* DrawingParameter::AsDoubleArray(uint32_t& array_size) const
{
    return AsScalarArray<double, eBasic_FP64>(array_size);
}

void DrawingParameter::AsDoubleArray(const double* val, uint32_t array_size)
{
    AsScalarArray<double, eBasic_FP64>(val, array_size);
}

const double2* DrawingParameter::AsDouble2Array(uint32_t& array_size) const
{
    return AsVectorArray<double2, eBasic_FP64, 2>(array_size);
}

void DrawingParameter::AsDouble2Array(const double2* val, uint32_t array_size)
{
    AsVectorArray<double2, eBasic_FP64, 2>(val, array_size);
}

const double3* DrawingParameter::AsDouble3Array(uint32_t& array_size) const
{
    return AsVectorArray<double3, eBasic_FP64, 3>(array_size);
}

void DrawingParameter::AsDouble3Array(const double3* val, uint32_t array_size)
{
    AsVectorArray<double3, eBasic_FP64, 3>(val, array_size);
}

const double4* DrawingParameter::AsDouble4Array(uint32_t& array_size) const
{
    return AsVectorArray<double4, eBasic_FP64, 4>(array_size);
}

void DrawingParameter::AsDouble4Array(const double4* val, uint32_t array_size)
{
    AsVectorArray<double4, eBasic_FP64, 4>(val, array_size);
}

const double2x2* DrawingParameter::AsDouble2x2Array(uint32_t& array_size) const
{
    return AsMatrixArray<double2x2, eBasic_FP64, 2, 2>(array_size);
}

void DrawingParameter::AsDouble2x2Array(const double2x2* val, uint32_t array_size)
{
    AsMatrixArray<double2x2, eBasic_FP64, 2, 2>(val, array_size);
}

const double3x3* DrawingParameter::AsDouble3x3Array(uint32_t& array_size) const
{
    return AsMatrixArray<double3x3, eBasic_FP64, 3, 3>(array_size);
}

void DrawingParameter::AsDouble3x3Array(const double3x3* val, uint32_t array_size)
{
    AsMatrixArray<double3x3, eBasic_FP64, 3, 3>(val, array_size);
}

const double4x4* DrawingParameter::AsDouble4x4Array(uint32_t& array_size) const
{
    return AsMatrixArray<double4x4, eBasic_FP64, 4, 4>(array_size);
}

void DrawingParameter::AsDouble4x4Array(const double4x4* val, uint32_t array_size)
{
    AsMatrixArray<double4x4, eBasic_FP64, 4, 4>(val, array_size);
}

const DrawingRawTexture* const* DrawingParameter::AsTextureArray(uint32_t& array_size) const
{
    assert(GetObjectType() == eObject_Texture);

    return AsValueArray<const DrawingRawTexture*>(array_size);
}

void DrawingParameter::AsTextureArray(const DrawingRawTexture** pTexture, uint32_t array_size)
{
    assert(GetObjectType() == eObject_Texture);

    AsValueArray<const DrawingRawTexture*>(pTexture, array_size);
}

const DrawingRawSamplerState* const* DrawingParameter::AsSamplerArray(uint32_t& array_size) const
{
    assert(GetObjectType() == eObject_Sampler);

    return AsValueArray<const DrawingRawSamplerState*>(array_size);
}

void DrawingParameter::AsSamplerArray(const DrawingRawSamplerState** pState, uint32_t array_size)
{
    assert(GetObjectType() == eObject_Sampler);

    AsValueArray<const DrawingRawSamplerState*>(pState, array_size);
}

void DrawingParameter::CreateParameter(uint32_t type, void* pInitVal)
{
    auto objectType = GetObjectType(type);

    if (objectType == eObject_Value)
        CreateValueParameter(type, pInitVal);
    else
        CreateObjectParameter(type, pInitVal);
}

void DrawingParameter::CreateObjectParameter(uint32_t type, void* pInitVal)
{
    m_type = type;
    UpdateValue(pInitVal, sizeof(void*));

    m_bDirty = true;
}

void DrawingParameter::CreateValueParameter(uint32_t type,  void* pInitVal)
{
    auto dataSetType = GetDataSetType(type);

    switch (dataSetType)
    {
        case eDataSet_Scalar: CreateScalarParameter(type, pInitVal); break;
        case eDataSet_Vector: CreateVectorParameter(type, pInitVal); break;
        case eDataSet_Matrix: CreateMatrixParameter(type, pInitVal); break;
        case eDataSet_Struct: CreateStructParameter(type, pInitVal); break;
    }
}

void DrawingParameter::CreateScalarParameter(uint32_t type, void* pInitVal)
{
    auto basicType = GetBasicType(type);
    auto arraySize = GetArraySize(type);

    if (arraySize == 0)
        arraySize = 1;

    m_type = type;
    UpdateValue(pInitVal, BasicTypeSize[basicType] * arraySize);

    m_bDirty = true;
}

void DrawingParameter::CreateVectorParameter(uint32_t type, void* pInitVal)
{
    auto basicType = GetBasicType(type);
    auto rowSize = GetRowSize(type);
    auto arraySize = GetArraySize(type);

    if (arraySize == 0)
        arraySize = 1;

    m_type = type;
    UpdateValue(pInitVal, BasicTypeSize[basicType] * rowSize * arraySize);

    m_bDirty = true;
}

void DrawingParameter::CreateMatrixParameter(uint32_t type, void* pInitVal)
{
    auto basicType = GetBasicType(type);
    auto rowSize = GetRowSize(type);
    auto colSize = GetColSize(type);
    auto arraySize = GetArraySize(type);

    if (arraySize == 0)
        arraySize = 1;

    m_type = type;
    UpdateValue(pInitVal, BasicTypeSize[basicType] * rowSize * colSize * arraySize);

    m_bDirty = true;
}

void DrawingParameter::CreateStructParameter(uint32_t type, void* pInitVal)
{
    auto arraySize = GetArraySize(type);
    auto structSize = GetStructSize(type);

    if (arraySize == 0)
        arraySize = 1;

    m_type = type;
    UpdateValue(pInitVal, structSize * arraySize);

    m_bDirty = true;
}

void DrawingParameter::UpdateValue(void* pInitVal, uint32_t size)
{
    m_size = size;
    m_pValue = new char[m_size];

    if (pInitVal != nullptr)
        memcpy(m_pValue, pInitVal, m_size);
    else
        memset(m_pValue, 0, m_size);
}

template <typename T>
const T& DrawingParameter::AsValue() const
{
    return *(T*)m_pValue;
}

template <typename T>
void DrawingParameter::AsValue(const T& val)
{
    if (memcmp(m_pValue, &val, sizeof(T)) != 0)
    {
        *(T*)m_pValue = val;
        m_bDirty = true;
    }
}

template <typename T, EBasicType type>
const T& DrawingParameter::AsScalar() const
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Scalar);
    assert(GetBasicType() == type);

    return AsValue<T>();
}

template <typename T, EBasicType type>
void DrawingParameter::AsScalar(const T& val)
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Scalar);
    assert(GetBasicType() == type);

    AsValue<T>(val);
}

template <typename T, EBasicType type, uint32_t size>
const T& DrawingParameter::AsVector() const
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Vector);
    assert(GetBasicType() == type);
    assert(GetVectorSize() == size);

    return AsValue<T>();
}

template <typename T, EBasicType type, uint32_t size>
void DrawingParameter::AsVector(const T& val)
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Vector);
    assert(GetBasicType() == type);
    assert(GetVectorSize() == size);

    AsValue<T>(val);
}

template <typename T, EBasicType type, uint32_t row_size, uint32_t col_size>
const T& DrawingParameter::AsMatrix() const
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Matrix);
    assert(GetBasicType() == type);
    assert(GetRowSize() == row_size);
    assert(GetColSize() == col_size);

    return AsValue<T>();
}

template <typename T, EBasicType type, uint32_t row_size, uint32_t col_size>
void DrawingParameter::AsMatrix(const T& val)
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Matrix);
    assert(GetBasicType() == type);
    assert(GetRowSize() == row_size);
    assert(GetColSize() == col_size);

    AsValue<T>(val);
}

template <typename T>
const T* DrawingParameter::AsValueArray(uint32_t& array_size) const
{
    array_size = GetArraySize();
    return (T*)m_pValue;
}

template <typename T>
void DrawingParameter::AsValueArray(const T* val, uint32_t array_size)
{
    assert(GetArraySize() == array_size);

    if (memcmp(m_pValue, val, sizeof(T) * array_size) != 0)
    {
        memcpy(m_pValue, val, m_size);
        m_bDirty = true;
    }
}

template <typename T, EBasicType type>
const T* DrawingParameter::AsScalarArray(uint32_t& array_size) const
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Scalar);
    assert(GetBasicType() == type);

    return AsValueArray<T>(array_size);
}

template <typename T, EBasicType type>
void DrawingParameter::AsScalarArray(const T* val, uint32_t array_size)
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Scalar);
    assert(GetBasicType() == type);

    AsValueArray<T>(val, array_size);
}

template <typename T, EBasicType type, uint32_t size>
const T* const DrawingParameter::AsVectorArray(uint32_t& array_size) const
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Vector);
    assert(GetBasicType() == type);
    assert(GetVectorSize() == size);

    return AsValueArray<T>(array_size);
}

template <typename T, EBasicType type, uint32_t size>
void DrawingParameter::AsVectorArray(const T* val, uint32_t array_size)
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Vector);
    assert(GetBasicType() == type);
    assert(GetVectorSize() == size);

    AsValueArray<T>(val, array_size);
}

template <typename T, EBasicType type, uint32_t row_size, uint32_t col_size>
const T* const DrawingParameter::AsMatrixArray(uint32_t& array_size) const
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Matrix);
    assert(GetBasicType() == type);
    assert(GetRowSize() == row_size);
    assert(GetColSize() == col_size);

    return AsValueArray<T>(array_size);
}

template <typename T, EBasicType type, uint32_t row_size, uint32_t col_size>
void DrawingParameter::AsMatrixArray(const T* val, uint32_t array_size)
{
    assert(GetObjectType() == eObject_Value);
    assert(GetDataSetType() == eDataSet_Matrix);
    assert(GetBasicType() == type);
    assert(GetRowSize() == row_size);
    assert(GetColSize() == col_size);

    AsValueArray<T>(val, array_size);
}

DrawingParameterSet::DrawingParameterSet()
{
}

DrawingParameterSet::~DrawingParameterSet()
{
    Clear();
}

void DrawingParameterSet::Add(std::shared_ptr<DrawingParameter> pParam)
{
    assert(pParam != nullptr);
    m_pParamList.emplace_back(pParam);
}

void DrawingParameterSet::AddSet(const DrawingParameterSet& paramSet)
{
    std::for_each(paramSet.m_pParamList.cbegin(), paramSet.m_pParamList.cend(), [this](std::shared_ptr<DrawingParameter> pParam)
    {
        Add(pParam);
    });
}

void DrawingParameterSet::AddUnique(std::shared_ptr<DrawingParameter> pParam)
{
    if (!Contains(pParam))
        Add(pParam);
}

void DrawingParameterSet::AddSetUnique(const DrawingParameterSet& paramSet)
{
    std::for_each(paramSet.m_pParamList.cbegin(), paramSet.m_pParamList.cend(), [this](std::shared_ptr<DrawingParameter> pParam)
    {
        if (!Contains(pParam))
            Add(pParam);
    });
}

void DrawingParameterSet::Remove(std::shared_ptr<DrawingParameter> pParam)
{
    auto result = std::find(m_pParamList.cbegin(), m_pParamList.cend(), pParam);
    if (result != m_pParamList.cend())
        m_pParamList.erase(result);
}

void DrawingParameterSet::RemoveAt(int32_t index)
{
    assert(index >= 0 && index < Count());
    m_pParamList.erase(m_pParamList.cbegin() + index);
}

bool DrawingParameterSet::Contains(const std::shared_ptr<DrawingParameter> pParam) const
{
    auto result = std::find(m_pParamList.cbegin(), m_pParamList.cend(), pParam);
    return (result != m_pParamList.cend());
}

bool DrawingParameterSet::Contains(const std::shared_ptr<std::string> pName) const
{
    return IndexOfName(pName) != -1;
}

int32_t DrawingParameterSet::IndexOf(const std::shared_ptr<DrawingParameter> pParam) const
{
    assert(pParam != nullptr);

    auto it = std::find(m_pParamList.cbegin(), m_pParamList.cend(), pParam);

    return it != m_pParamList.cend() ? static_cast<int32_t>(it - m_pParamList.cbegin()) : -1;
}

int32_t DrawingParameterSet::IndexOfName(const std::shared_ptr<std::string> pName) const
{
    assert(pName != nullptr);

    auto it = std::find_if(m_pParamList.cbegin(), m_pParamList.cend(), [pName](const std::shared_ptr<DrawingParameter> pParam)
    {
        assert(pParam != nullptr);
        return pParam->GetName()->compare(*pName) == 0;
    });

    return it != m_pParamList.cend() ? static_cast<int32_t>(it - m_pParamList.cbegin()) : -1;
}

int32_t DrawingParameterSet::IndexOfSemantic(const std::shared_ptr<std::string> pSemantic) const
{
    assert(pSemantic != nullptr);

    auto it = std::find_if(m_pParamList.cbegin(), m_pParamList.cend(), [pSemantic](const std::shared_ptr<DrawingParameter> pParam)
    {
        assert(pParam != nullptr);

        const auto lSemantic = pParam->GetSemantic();
        return lSemantic != nullptr && lSemantic->compare(*pSemantic.get());
    });

    return it != m_pParamList.cend() ? static_cast<int32_t>(it - m_pParamList.cbegin()) : -1;
}

void DrawingParameterSet::Clear()
{
    m_pParamList.clear();
}

int32_t DrawingParameterSet::Count() const
{
    return static_cast<int32_t>(m_pParamList.size());
}