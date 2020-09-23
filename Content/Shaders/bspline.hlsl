//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma pack_matrix(row_major)

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 transform;
    float2 tessFactor;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct HSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct HSConstOutput
{
    float factors[2]: SV_TessFactor;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct GSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};


Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

VSOutput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
    VSOutput result;
    
    result.position = mul(position, transform);
    result.uv = uv;

    return result;
}

HSConstOutput HSConst()
{
    HSConstOutput output;
    output.factors[0] = tessFactor.x;
    output.factors[1] = tessFactor.y;
    return output;
}

[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(4)]
[patchconstantfunc("HSConst")]
HSOutput HSMain(InputPatch<VSOutput, 4> input, uint id : SV_OutputControlPointID)
{
    HSOutput output;
    output.position = input[id].position;
    output.uv = input[id].uv;
    return output;
}

// B(t) = (1-t)^2 * P0 + 2t*(1-t)*P1 + t*t*P2
float4 EvaluateBezier2(const OutputPatch<HSOutput, 3> patchs, float t)
{
    float coff0 = (1 - t) * (1 - t);
    float coff1 = 2 * t * (1 - t);
    float coff2 = t * t;
    return coff0 * patchs[0].position + coff1 * patchs[1].position + coff2 * patchs[2].position;
}

float4 EvaluateBSpline4(const OutputPatch<HSOutput, 4> patchs, float t)
{
    float4x4 basisMatrix = float4x4
    (
        -1 / 6.0, 3 / 6.0, -3 / 6.0, 1 / 6.0,
         3 / 6.0, -6 / 6.0, 3 / 6.0, 0,
        -3 / 6.0, 0, 3 / 6.0, 0,
         1 / 6.0, 4 / 6.0, 1 / 6.0, 0
    );
    
    float4 basis = mul(float4(t * t * t, t * t, t, 1), basisMatrix);
    return patchs[0].position * basis[0] + patchs[1].position * basis[1] + patchs[2].position * basis[2] + patchs[3].position * basis[3];
}

[domain("isoline")]
DSOutput DSMain(HSConstOutput input, OutputPatch<HSOutput, 4> patchs, float2 uv : SV_DomainLocation)
{
    DSOutput output;
    output.position = EvaluateBSpline4(patchs, uv.x);
    output.position.y += uv.y*0.2;
    output.uv = float2(0.0f, 0.0f);
    return output;
}

[maxvertexcount(4)]
void GSMain(line DSOutput vertices[2], inout TriangleStream<GSOutput> stream)
{
    float radius = 0.5f;
    DSOutput result;
    result.uv = float2(0.0f, 0.0f);
    result.position = vertices[0].position + float4(0.0f, +radius, 0.0f, 0.0f);
    stream.Append(result);
    result.position = vertices[0].position + float4(0.0f, -radius, 0.0f, 0.0f);
    stream.Append(result);
    result.position = vertices[0].position + float4( +radius, 0.0f, 0.0f, 0.0f);
    stream.Append(result);
    result.position = vertices[0].position + float4( -radius, 0.0f, 0.0f, 0.0f);
    stream.Append(result);

    stream.RestartStrip();
}

float4 PSMain(GSOutput input) : SV_TARGET
{
    //return g_texture.Sample(g_sampler, input.uv);
    return float4(1, 0, 0, 1);

}
