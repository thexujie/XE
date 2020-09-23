
#pragma pack_matrix(row_major)

#include "Common.hlsli"


Texture2D<float4> Image;

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 Texcoord: TEXCOORD;
};

float4 PSMain(VSOutput Input) : SV_TARGET
{
    float4 Color = Image.Sample(BilinearClampedSampler, Input.Texcoord);
    return float4(Color.rgb, 1);
}
