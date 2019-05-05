#pragma once

#include <string>
#include <memory>
#include <vector>

#include "DrawingType.h"

namespace Engine
{
    enum
    {
        Object_Type_Bits = 4,
        Object_Type_Offset = 28,

        DataSet_Type_Bits = 4,
        DataSet_Type_Offset = 24,

        Basic_Type_Bits = 4,
        Basic_Type_Offset = 20,

        Array_Size_Bits = 8,
        Array_Size_Offset = 12,

        Row_Size_Bits = 4,
        Row_Size_Offset = 8,

        Col_Size_Bits = 4,
        Col_Size_Offset = 4,

        Struct_Size_Bits = 4,
        Struct_Size_Offset = 0,
    };

    enum EObjectType
    {
        eObject_Value = 0,
        eObject_Texture,
        eObject_Buffer,
        eObject_RWBuffer,
        eObject_TexBuffer,
        eObject_Sampler,
    };

    enum EDataSetType
    {
        eDataSet_Scalar = 0,
        eDataSet_Vector,
        eDataSet_Matrix,
        eDataSet_Struct,
        eDataSet_Object,
    };

    enum EBasicType
    {
        eBasic_Bool = 0,
        eBasic_UInt32,
        eBasic_Int32,
        eBasic_FP32,
        eBasic_FP64,

        eBasic_Count,
    };

    static const uint32_t BasicTypeSize[eBasic_Count];

    #define SET_BITS(val, bits, offset) ((val & ((0x1 << bits) - 1)) << offset)

    #define COMPOSE_TYPE(object, dataset, basic, array, row, col)       \
        SET_BITS(object, Object_Type_Bits, Object_Type_Offset) |        \
        SET_BITS(dataset, DataSet_Type_Bits, DataSet_Type_Offset) |     \
        SET_BITS(basic, Basic_Type_Bits, Basic_Type_Offset) |           \
        SET_BITS(array, Array_Size_Bits, Array_Size_Offset) |           \
        SET_BITS(row, Row_Size_Bits, Row_Size_Offset) |                 \
        SET_BITS(col, Col_Size_Bits, Col_Size_Offset)

    #define COMPOSE_STRUCT_TYPE(object, dataset, basic, array, struct)  \
        SET_BITS(object, Object_Type_Bits, Object_Type_Offset) |        \
        SET_BITS(dataset, DataSet_Type_Bits, DataSet_Type_Offset) |     \
        SET_BITS(basic, Basic_Type_Bits, Basic_Type_Offset) |           \
        SET_BITS(array, Array_Size_Bits, Array_Size_Offset) |           \
        SET_BITS(struct, Struct_Size_Bits, Struct_Size_Offset)

    #define COMPOSE_SCALAR(basic)           COMPOSE_TYPE(eObject_Value, eDataSet_Scalar, basic, 0, 0, 0)
    #define COMPOSE_VECTOR(basic, size)     COMPOSE_TYPE(eObject_Value, eDataSet_Vector, basic, 0, size, 0)
    #define COMPOSE_MATRIX(basic, row, col) COMPOSE_TYPE(eObject_Value, eDataSet_Matrix, basic, 0, row, col)
    #define COMPOSE_OBJECT(object)          COMPOSE_TYPE(object, eDataSet_Object, eBasic_FP32, 0, 0, 0)
    #define COMPOSE_STRUCT(size)            COMPOSE_STRUCT_TYPE(eObject_Value, eDataSet_Struct, eBasic_FP32, 0, size)

    enum EParamType
    {
        EParam_Bool = COMPOSE_SCALAR(eBasic_Bool),
        EParam_UInt = COMPOSE_SCALAR(eBasic_UInt32),
        EParam_Int = COMPOSE_SCALAR(eBasic_Int32),
        EParam_Float = COMPOSE_SCALAR(eBasic_FP32),
        EParam_Double = COMPOSE_SCALAR(eBasic_FP64),

        EParam_Bool2 = COMPOSE_VECTOR(eBasic_Bool, 2),
        EParam_UInt2 = COMPOSE_VECTOR(eBasic_UInt32, 2),
        EParam_Int2 = COMPOSE_VECTOR(eBasic_Int32, 2),
        EParam_Float2 = COMPOSE_VECTOR(eBasic_FP32, 2),
        EParam_Double2 = COMPOSE_VECTOR(eBasic_FP64, 2),

        EParam_Bool3 = COMPOSE_VECTOR(eBasic_Bool, 3),
        EParam_UInt3 = COMPOSE_VECTOR(eBasic_UInt32, 3),
        EParam_Int3 = COMPOSE_VECTOR(eBasic_Int32, 3),
        EParam_Float3 = COMPOSE_VECTOR(eBasic_FP32, 3),
        EParam_Double3 = COMPOSE_VECTOR(eBasic_FP64, 3),

        EParam_Bool4 = COMPOSE_VECTOR(eBasic_Bool, 4),
        EParam_UInt4 = COMPOSE_VECTOR(eBasic_UInt32, 4),
        EParam_Int4 = COMPOSE_VECTOR(eBasic_Int32, 4),
        EParam_Float4 = COMPOSE_VECTOR(eBasic_FP32, 4),
        EParam_Double4 = COMPOSE_VECTOR(eBasic_FP64, 4),

        EParam_Bool3x3 = COMPOSE_MATRIX(eBasic_Bool, 3, 3),
        EParam_UInt3x3 = COMPOSE_MATRIX(eBasic_UInt32, 3, 3),
        EParam_Int3x3 = COMPOSE_MATRIX(eBasic_Int32, 3, 3),
        EParam_Float3x3 = COMPOSE_MATRIX(eBasic_FP32, 3, 3),
        EParam_Double3x3 = COMPOSE_MATRIX(eBasic_FP64, 3, 3),

        EParam_Bool4x4 = COMPOSE_MATRIX(eBasic_Bool, 4, 4),
        EParam_UInt4x4 = COMPOSE_MATRIX(eBasic_UInt32, 4, 4),
        EParam_Int4x4 = COMPOSE_MATRIX(eBasic_Int32, 4, 4),
        EParam_Float4x4 = COMPOSE_MATRIX(eBasic_FP32, 4, 4),
        EParam_Double4x4 = COMPOSE_MATRIX(eBasic_FP64, 4, 4),

        EParam_Texture = COMPOSE_OBJECT(eObject_Texture),
        EParam_Buffer = COMPOSE_OBJECT(eObject_Buffer),
        EParam_RWBuffer = COMPOSE_OBJECT(eObject_RWBuffer),
        EParam_TexBuffer = COMPOSE_OBJECT(eObject_TexBuffer),
        EParam_Sampler = COMPOSE_OBJECT(eObject_Sampler),

        EParam_Invalid = 0xffffffff
    };

    class DrawingRawTexture;
    class DrawingRawTexBuffer;
    class DrawingRawRWBuffer;
    class DrawingRawSamplerState;

    class DrawingParameter
    {
    public:
        DrawingParameter();
        DrawingParameter(const std::shared_ptr<std::string> pName, uint32_t type, void* pInitVal, const std::shared_ptr<std::string> pSemantic);
        virtual ~DrawingParameter();

        static const uint32_t GetBitsValue(uint32_t val, uint32_t bits, uint32_t offset);
        static const uint32_t GetObjectType(uint32_t type);
        static const uint32_t GetDataSetType(uint32_t type);
        static const uint32_t GetBasicType(uint32_t type);
        static const uint32_t GetArraySize(uint32_t type);
        static const uint32_t GetRowSize(uint32_t type);
        static const uint32_t GetColSize(uint32_t type);
        static const uint32_t GetStructSize(uint32_t type);

        std::shared_ptr<std::string> GetName() const;
        void SetName(std::shared_ptr<std::string> pName);

        std::shared_ptr<std::string> GetSemantic() const;
        void SetSemantic(std::shared_ptr<std::string> pSemantic);

        uint32_t GetSize() const;
        void SetSize(uint32_t size);

        uint32_t GetType() const;
        void SetType(uint32_t type);

        const void* const GetValuePtr() const;
        uint32_t GetValueSize() const;

        void SetValue(const void* pInitVal, uint32_t size);

        uint32_t GetObjectType() const;
        uint32_t GetDataSetType() const;
        uint32_t GetBasicType() const;
        uint32_t GetArraySize() const;
        uint32_t GetRowSize() const;
        uint32_t GetColSize() const;
        uint32_t GetStructSize() const;
        uint32_t GetVectorSize() const;

        const bool& AsBool() const;
        void AsBool(const bool& val);
        const bool2& AsBool2() const;
        void AsBool2(const bool2& val);
        const bool3& AsBool3() const;
        void AsBool3(const bool3& val);
        const bool4& AsBool4() const;
        void AsBool4(const bool4& val);

        const uint32_t& AsUInt() const;
        void AsUInt(const uint32_t& val);
        const uint2& AsUInt2() const;
        void AsUInt2(const uint2& val);
        const uint3& AsUInt3() const;
        void AsUInt3(const uint3& val);
        const uint4& AsUInt4() const;
        void AsUInt4(const uint4& val);

        const int32_t& AsInt() const;
        void AsInt(const int32_t& val);
        const int2& AsInt2() const;
        void AsInt2(const int2& val);
        const int3& AsInt3() const;
        void AsInt3(const int3& val);
        const int4& AsInt4() const;
        void AsInt4(const int4& val);

        const float& AsFloat() const;
        void AsFloat(const float& val);
        const float2& AsFloat2() const;
        void AsFloat2(const float2& val);
        const float3& AsFloat3() const;
        void AsFloat3(const float3& val);
        const float4& AsFloat4() const;
        void AsFloat4(const float4& val);
    
        const float2x2& AsFloat2x2() const;
        void AsFloat2x2(const float2x2& val);
        const float3x3& AsFloat3x3() const;
        void AsFloat3x3(const float3x3& val);
        const float4x4& AsFloat4x4() const;
        void AsFloat4x4(const float4x4& val);

        const double& AsDouble() const;
        void AsDouble(const double& val);
        const double2& AsDouble2() const;
        void AsDouble2(const double2& val);
        const double3& AsDouble3() const;
        void AsDouble3(const double3& val);
        const double4& AsDouble4() const;
        void AsDouble4(const double4& val);
    
        const double2x2& AsDouble2x2() const;
        void AsDouble2x2(const double2x2& val);
        const double3x3& AsDouble3x3() const;
        void AsDouble3x3(const double3x3& val);
        const double4x4& AsDouble4x4() const;
        void AsDouble4x4(const double4x4& val);

        const DrawingRawTexture* AsTexture() const;
        void AsTexture(const DrawingRawTexture* pTexture);

        const DrawingRawTexBuffer* AsBuffer() const;
        void AsBuffer(const DrawingRawTexBuffer* pBuffer);

        const DrawingRawRWBuffer* AsRWBuffer() const;
        void AsRWBuffer(const DrawingRawRWBuffer* pRWBuffer);

        const DrawingRawTexBuffer* AsTexBuffer() const;
        void AsTexBuffer(const DrawingRawTexBuffer* pTexBuffer);

        const DrawingRawSamplerState* AsSampler() const;
        void AsSampler(const DrawingRawSamplerState* pState);

        const bool* AsBoolArray(uint32_t& array_size) const;
        void AsBoolArray(const bool* val, uint32_t array_size);
        const bool2* AsBool2Array(uint32_t& array_size) const;
        void AsBool2Array(const bool2* val, uint32_t array_size);
        const bool3* AsBool3Array(uint32_t& array_size) const;
        void AsBool3Array(const bool3* val, uint32_t array_size);
        const bool4* AsBool4Array(uint32_t& array_size) const;
        void AsBool4Array(const bool4* val, uint32_t array_size);

        const uint32_t* AsUIntArray(uint32_t& array_size) const;
        void AsUIntArray(const uint32_t* val, uint32_t array_size);
        const uint2* AsUInt2Array(uint32_t& array_size) const;
        void AsUInt2Array(const uint2* val, uint32_t array_size);
        const uint3* AsUInt3Array(uint32_t& array_size) const;
        void AsUInt3Array(const uint3* val, uint32_t array_size);
        const uint4* AsUInt4Array(uint32_t& array_size) const;
        void AsUInt4Array(const uint4* val, uint32_t array_size);

        const int32_t* AsIntArray(uint32_t& array_size) const;
        void AsIntArray(const int32_t* val, uint32_t array_size);
        const int2* AsInt2Array(uint32_t& array_size) const;
        void AsInt2Array(const int2* val, uint32_t array_size);
        const int3* AsInt3Array(uint32_t& array_size) const;
        void AsInt3Array(const int3* val, uint32_t array_size);
        const int4* AsInt4Array(uint32_t& array_size) const;
        void AsInt4Array(const int4* val, uint32_t array_size);

        const float* AsFloatArray(uint32_t& array_size) const;
        void AsFloatArray(const float* val, uint32_t array_size);
        const float2* AsFloat2Array(uint32_t& array_size) const;
        void AsFloat2Array(const float2* val, uint32_t array_size);
        const float3* AsFloat3Array(uint32_t& array_size) const;
        void AsFloat3Array(const float3* val, uint32_t array_size);
        const float4* AsFloat4Array(uint32_t& array_size) const;
        void AsFloat4Array(const float4* val, uint32_t array_size);
    
        const float2x2* AsFloat2x2Array(uint32_t& array_size) const;
        void AsFloat2x2Array(const float2x2* val, uint32_t array_size);
        const float3x3* AsFloat3x3Array(uint32_t& array_size) const;
        void AsFloat3x3Array(const float3x3* val, uint32_t array_size);
        const float4x4* AsFloat4x4Array(uint32_t& array_size) const;
        void AsFloat4x4Array(const float4x4* val, uint32_t array_size);

        const double* AsDoubleArray(uint32_t& array_size) const;
        void AsDoubleArray(const double* val, uint32_t array_size);
        const double2* AsDouble2Array(uint32_t& array_size) const;
        void AsDouble2Array(const double2* val, uint32_t array_size);
        const double3* AsDouble3Array(uint32_t& array_size) const;
        void AsDouble3Array(const double3* val, uint32_t array_size);
        const double4* AsDouble4Array(uint32_t& array_size) const;
        void AsDouble4Array(const double4* val, uint32_t array_size);
    
        const double2x2* AsDouble2x2Array(uint32_t& array_size) const;
        void AsDouble2x2Array(const double2x2* val, uint32_t array_size);
        const double3x3* AsDouble3x3Array(uint32_t& array_size) const;
        void AsDouble3x3Array(const double3x3* val, uint32_t array_size);
        const double4x4* AsDouble4x4Array(uint32_t& array_size) const;
        void AsDouble4x4Array(const double4x4* val, uint32_t array_size);

        const DrawingRawTexture* const* AsTextureArray(uint32_t& array_size) const;
        void AsTextureArray(const DrawingRawTexture** pTexture, uint32_t array_size);

        const DrawingRawSamplerState* const* AsSamplerArray(uint32_t& array_size) const;
        void AsSamplerArray(const DrawingRawSamplerState** pState, uint32_t array_size);

    private:
        void CreateParameter(uint32_t type, void* pInitVal);
        void CreateObjectParameter(uint32_t type, void* pInitVal);
        void CreateValueParameter(uint32_t type,  void* pInitVal);
        void CreateScalarParameter(uint32_t type, void* pInitVal);
        void CreateVectorParameter(uint32_t type, void* pInitVal);
        void CreateMatrixParameter(uint32_t type, void* pInitVal);
        void CreateStructParameter(uint32_t type, void* pInitVal);

        void UpdateValue(void* pInitVal, uint32_t size);

        template <typename T>
        const T& AsValue() const;
        template <typename T>
        void AsValue(const T& val);

        template <typename T, EBasicType type>
        const T& AsScalar() const;
        template <typename T, EBasicType type>
        void AsScalar(const T& val);

        template <typename T, EBasicType type, uint32_t size>
        const T& AsVector() const;
        template <typename T, EBasicType type, uint32_t size>
        void AsVector(const T& val);

        template <typename T, EBasicType type, uint32_t row_size, uint32_t col_size>
        const T& AsMatrix() const;
        template <typename T, EBasicType type, uint32_t row_size, uint32_t col_size>
        void AsMatrix(const T& val);

        template <typename T>
        const T* AsValueArray(uint32_t& array_size) const;
        template <typename T>
        void AsValueArray(const T* val, uint32_t array_size);

        template <typename T, EBasicType type>
        const T* AsScalarArray(uint32_t& array_size) const;
        template <typename T, EBasicType type>
        void AsScalarArray(const T* val, uint32_t array_size);

        template <typename T, EBasicType type, uint32_t size>
        const T* const AsVectorArray(uint32_t& array_size) const;
        template <typename T, EBasicType type, uint32_t size>
        void AsVectorArray(const T* val, uint32_t array_size);

        template <typename T, EBasicType type, uint32_t row_size, uint32_t col_size>
        const T* const AsMatrixArray(uint32_t& array_size) const;
        template <typename T, EBasicType type, uint32_t row_size, uint32_t col_size>
        void AsMatrixArray(const T* val, uint32_t array_size);

    private:
        std::shared_ptr<std::string> m_pName;
        std::shared_ptr<std::string> m_pSemantic;
        void* m_pValue;
        uint32_t m_size;
        uint32_t m_type;
    };

    class DrawingParameterSet
    {
    public:
        DrawingParameterSet();
        virtual ~DrawingParameterSet();

        virtual void Add(std::shared_ptr<DrawingParameter> pParam);
        virtual void AddSet(const DrawingParameterSet& paramSet);

        virtual void AddUnique(std::shared_ptr<DrawingParameter> pParam);
        virtual void AddSetUnique(const DrawingParameterSet& paramSet);

        virtual void Remove(std::shared_ptr<DrawingParameter> pParam);
        virtual void RemoveAt(int32_t index);

        virtual bool Contains(const std::shared_ptr<DrawingParameter> pParam) const;
        virtual bool Contains(const std::shared_ptr<std::string> pName) const;

        virtual int32_t IndexOf(const std::shared_ptr<DrawingParameter> pParam) const;
        virtual int32_t IndexOfName(const std::shared_ptr<std::string> pName) const;
        virtual int32_t IndexOfSemantic(const std::shared_ptr<std::string> pSemantic) const;

        virtual void Clear();
        virtual int32_t Count() const;

        std::shared_ptr<DrawingParameter> operator[] (const int32_t index) const
        {
            assert(index >= 0 && index < static_cast<int32_t>(m_pParamList.size()));
            return m_pParamList[index];
        }

        std::shared_ptr<DrawingParameter> operator[] (const std::shared_ptr<std::string> pName) const
        {
            return operator[](IndexOfName(pName));
        }

    private:
        typedef std::vector<std::shared_ptr<DrawingParameter>> ParameterList;
        ParameterList m_pParamList;
    };
}