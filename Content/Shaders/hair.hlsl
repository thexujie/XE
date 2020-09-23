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
    float4x4 worldTransform;
    float4x4 viewprojTransform;
    float2 tessFactor;
    float2 _unused;
    float3 eyePosition;
    float1 _unused0;
    float3 lightVector;
    float1 _unused1;
};

struct VSInput
{
    uint4 pointIndex : PATCH_INDEX;
    uint strandIndex : STRAND_INDEX;
};

struct VSOutput
{
    uint4 pointIndex : PATCH_INDEX;
    uint strandIndex : STRAND_INDEX;
};

struct HSOutput
{
    uint4 pointIndex : PATCH_INDEX;
};

struct HSConstOutput
{
    float factors[2] : SV_TessFactor;
    uint strandIndex : STRAND_INDEX;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float3 tangent : TANGENT;
    float width : HAIR_WIDTH;
};

struct GSOutput
{
    float4 position : SV_POSITION;
    float3 tangent : TANGENT;
};

struct HairVertex
{
    float3 tangentY;
    float2 v;
};

Buffer<float4> g_positions : register(t0);
Buffer<float4> g_tangents: register(t1);
Buffer<float2> g_coordJitters : register(t2);

VSOutput VSMain(VSInput input)
{
    VSOutput result;
    result.pointIndex = input.pointIndex;
    result.strandIndex = input.strandIndex;
    return result;
}

HSConstOutput HSConst(InputPatch<VSOutput, 1> input)
{
    HSConstOutput output;
    output.factors[0] = tessFactor.x;
    output.factors[1] = tessFactor.y;
    output.strandIndex = input[0].strandIndex;
    return output;
}

[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(1)]
[patchconstantfunc("HSConst")]
HSOutput HSMain(InputPatch<VSOutput, 1> input, uint id : SV_OutputControlPointID)
{
    HSOutput output;
    output.pointIndex = input[id].pointIndex;
    return output;
}

// B(t) = (1-t)^2 * P0 + 2t*(1-t)*P1 + t*t*P2
//float4 EvaluateBezier2(const OutputPatch<HSOutput, 1> patchs, float t)
//{
//    float coff0 = (1 - t) * (1 - t);
//    float coff1 = 2 * t * (1 - t);
//    float coff2 = t * t;
//    return coff0 * patchs[0] + coff1 * patchs[1]. + coff2 * patchs[2].position;
//}

void EvaluateBSpline4(const uint4 pointIndex, float t, out float4 position, out float3 tangentX, out float3 tangentY, out float v)
{
    row_major float4x4 basicMatrix = float4x4
    (
        -1/6.0,  3/6.0, -3/6.0,  1/6.0,
         3/6.0, -6/6.0,  3/6.0,  0,
        -3/6.0,  0,      3/6.0,  0,
         1/6.0,  4/6.0,  1/6.0,  0
    );
    
    float4 position0 = g_positions[pointIndex[0]];
    float4 position1 = g_positions[pointIndex[1]];
    float4 position2 = g_positions[pointIndex[2]];
    float4 position3 = g_positions[pointIndex[3]];

    float4 positionCoff = mul(float4(t * t * t, t * t, t, 1), basicMatrix);
    float4 tangentCoff = mul(float4(3 * t * t, 2 * t, 1, 0), basicMatrix);

    position = position0 * positionCoff[0] + position1 * positionCoff[1] + position2 * positionCoff[2] + position3 * positionCoff[3];


    tangentX = -normalize((position0 * tangentCoff[0] + position1 * tangentCoff[1] + position2 * tangentCoff[2] + position3 * tangentCoff[3]).xyz);

    float4 tangentY_org = g_tangents[pointIndex[0]] * positionCoff[0] + g_tangents[pointIndex[1]] * positionCoff[1] + g_tangents[pointIndex[2]] * positionCoff[2] + g_tangents[pointIndex[3]] * positionCoff[3];
    tangentY = normalize(tangentY_org.xyz);
    v = tangentY_org.w;
}

[domain("isoline")]
DSOutput DSMain(HSConstOutput input, OutputPatch<HSOutput, 1> patchs, float2 uv : SV_DomainLocation)
{
    DSOutput output;
    float3 tangentY;
    float v = 0.0f;
    EvaluateBSpline4(patchs[0].pointIndex, uv.x, output.position, output.tangent, tangentY, v);
    float3 tangentZ = cross(tangentY, output.tangent);
    float radius = 1.5f * lerp(1.0f, 0.005f, v);
    float2 coordJitter = g_coordJitters[(input.strandIndex * 64 + uv.y * 64) % 1024] * lerp(0.2, 1.2, v);
    output.position.xyz += uv.y * radius * tangentY * coordJitter.x + uv.y * radius * tangentZ * coordJitter.y;

    float width = 0.01f;
    output.width = lerp(width, width * 0.1f, v);
    return output;
}

[maxvertexcount(4)]
void GSMain(line DSOutput vertices[2], inout TriangleStream<GSOutput> stream)
{
    float3 tangent = (vertices[1].position.xyz - vertices[0].position.xyz);
    float3 cameraVector = eyePosition - vertices[0].position.xyz;
    float3 sideVector = normalize(cross(cameraVector, tangent));
    float3 width0 = sideVector * 0.5 * vertices[0].width;
    float3 width1 = sideVector * 0.5 * vertices[1].width;

    GSOutput result;
    result.position = mul(float4(vertices[0].position.xyz - width0, 1.0f), viewprojTransform);
    result.tangent = vertices[0].tangent;
    stream.Append(result);
    result.position = mul(float4(vertices[0].position.xyz + width0, 1.0f), viewprojTransform);
    result.tangent = vertices[0].tangent;
    stream.Append(result);
    result.position = mul(float4(vertices[1].position.xyz - width1, 1.0f), viewprojTransform);
    result.tangent = vertices[1].tangent;
    stream.Append(result);
    result.position = mul(float4(vertices[1].position.xyz + width1, 1.0f), viewprojTransform);
    result.tangent = vertices[1].tangent;
    stream.Append(result);

    stream.RestartStrip();
}

float4 PSMain(GSOutput input) : SV_TARGET
{
    //return g_texture.Sample(g_sampler, input.uv);
    float3 L = lightVector;
    float3 T = input.tangent;
    float3 V = normalize(eyePosition - input.position.xyz);
    float3 sideVec = normalize(cross(V, T));
    //re-create the normal vector:
    float3 N = normalize(cross(T, sideVec));

    T += 0.2 * N.xyz;
    T = normalize(T);
    
    float3 TPrimary = T;
    TPrimary += 0.3 * N;
    TPrimary = normalize(TPrimary);
	
    float3 TSecondary = T;
    TSecondary -= 0.3 * N;
    TSecondary = normalize(TSecondary);

    
   //kajiya and kay lighting
    //diffuse
    float TdotL = dot(T, L);
    float sinTL = sqrt(1 - TdotL * TdotL);

    //specular primary
    float TPdotL = dot(TPrimary, L);
    float sinTPL = sqrt(1 - TPdotL * TPdotL);
    float TPdotE = dot(TPrimary, V);
    float sinTPE = sqrt(1 - TPdotE * TPdotE);

    //specular secondary
    float TSdotL = dot(TSecondary, L);
    float sinTSL = sqrt(1 - TSdotL * TSdotL);
    float TSdotE = dot(TSecondary, V);
    float sinTSE = sqrt(1 - TSdotE * TSdotE);

    float lightDiffuse = 1 /*- shadow*/;
    float lightSpec = pow(lightDiffuse, 2);
    
    float specPrimary = abs(TPdotL * TPdotE + sinTPL * sinTPE);
    
    float3 baseColor = float3(111 / 255.0f, 77 / 255.0f, 42 / 255.0f);
    float3 specColor = float3(111 / 255.0f, 77 / 255.0f, 42 / 255.0f);
    float ksP = 0.4f;
    float ksS = 0.3f;
    float kd = 1.0f;
    float specPowerPrimary = 6.0f;
    float specPowerSecondary = 6.0f;
    float ksP_sparkles = 0.2f;
    float specPowerPrimarySparkles = 600;
    float ka = 0.3f;

    float3 color = baseColor;

    color = color //ambient light
            + lightDiffuse * (kd * sinTL * color) //diffuse light
            + lightSpec * (ksP * pow(specPrimary, specPowerPrimary).xxx) //primary specular light
            + lightSpec * (ksS * pow(abs((TSdotL * TSdotE + sinTSL * sinTSE)), specPowerSecondary).xxx) * specColor.xyz //secondary specular light                      
            + lightSpec * (ksP_sparkles * pow(specPrimary, specPowerPrimarySparkles).xxx) //sparkles
    ;

    return float4(color, 1);

}
