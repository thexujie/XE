//*********************************************************
// A compile TEST shader
//*********************************************************
#pragma pack_matrix(row_major)

cbuffer SceneParameters
{
    float4x4 SceneWorldViewProjectionTransform;
}

cbuffer PrimitiveParemeters
{
    float4x4 WorldTransform;
    float4x4 ViewProjectionTransform;
    float4x4 WorldViewProjectionTransform;
};

struct VSInput
{
    float3 Potision : POSITION;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
    uint PositionIndex : POSITION_INDEX;
};

struct VSOutput
{
    float4 Potision : SV_POSITION;
    float4 Color : TEST_COLOR;
    float2 UV0 : TEXCOORD0;
    float2 UV1 : TEXCOORD1;
};

Texture2D<float4> Image;
SamplerState LinerSampler;

Buffer<float3> Positions;

struct TestData
{
    float4 Position;
    float2 Position2;
    float2 Position3;
    //float4 Position3;
};

RWByteAddressBuffer RWByteAddress;
RWBuffer<float2> RWFloat2;
RWBuffer<float3> RWFloat3;
RWBuffer<float4> RWFloat4;
//RWBuffer<TestData> RWStruct; // error

StructuredBuffer<float> StructBufferFloat;
StructuredBuffer<TestData> StructBufferStruct;

VSOutput VSMain(VSInput Input)
{
    VSOutput Result;
    Result.Potision = mul(float4(Input.Potision, 1.0f), WorldViewProjectionTransform);
    Result.Potision = mul(Result.Potision, SceneWorldViewProjectionTransform) + float4(Positions[Input.PositionIndex], 1.0f);
    //Result.Potision = float4(Input.Potision, 1.0f);
    Result.Color = Image.SampleLevel(LinerSampler, Input.UV0, 0);
    RWByteAddress.Store(1.0f, 1);
    Result.Potision.xy += RWFloat2[Input.PositionIndex];
    Result.Potision.xy += RWFloat3[Input.PositionIndex].xy;
    Result.Potision.xy += RWFloat4[Input.PositionIndex].xy;
    //Result.Potision.xy += RWStruct[Input.PositionIndex].Position.xy;
    Result.Potision.x += StructBufferFloat[Input.PositionIndex];
    Result.Potision.x += StructBufferStruct[Input.PositionIndex].Position.x + StructBufferStruct[Input.PositionIndex].Position2.x + StructBufferStruct[Input.PositionIndex].Position3.x;
    return Result;
}

