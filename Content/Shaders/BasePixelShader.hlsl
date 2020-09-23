
#pragma pack_matrix(row_major)

#include "Common.hlsli"

cbuffer MaterialParemeter : register(RootBufferIndex_Material)
{
    float3 Color;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
};

float4 PSMain(VSOutput Input) : SV_TARGET
{
    return float4(1,0,0,1);
    return float4(Color.rgb, 1);
}
