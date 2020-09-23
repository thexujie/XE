
#pragma pack_matrix(row_major)

#include "Common.hlsli"

Texture2D<float4> Image : register(t3);
SamplerState LinerSampler : register(s0);

struct VSInput
{
    uint VertexId : SV_VertexID;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD;
};

VSOutput VSMain(VSInput Input)
{
    float3 Positions[] =
    {
        float3(0.0f, 0.0f, 0.0f),
    	float3(1.0f, 0.0f, 0.0f),
    	float3(0.0f, 1.0f, 0.0f),
    	float3(0.0f, 1.0f, 0.0f),
    	float3(1.0f, 0.0f, 0.0f),
    	float3(1.0f, 1.0f, 0.0f),
    };
	
    float2 Texcoords[] =
    {
    	float2(0.0f, 0.0f),
    	float2(1.0f, 0.0f),
    	float2(0.0f, 1.0f),
    	float2(0.0f, 1.0f),
    	float2(1.0f, 0.0f),
    	float2(1.0f, 1.0f),
    };

    VSOutput Output;
    Output.Position = float4(Positions[Input.VertexId], 1.0f);
    Output.Texcoord = Texcoords[Input.VertexId];
    return Output;
}

float4 PSMain(VSOutput Input) : SV_TARGET
{
    return Image.Sample(LinerSampler, Input.Texcoord);
}
