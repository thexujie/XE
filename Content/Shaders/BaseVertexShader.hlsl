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

#include "Common.hlsli"

cbuffer PrimitiveParemeter
{
    float4x4 LocalTransform;

    float4x4 WorldTransform;
    float4x4 ViewTransform;
    float4x4 ProjectionTransform;
    float4x4 ViewProjectionTransform;

    float4x4 WorldViewProjectionTransform;
};

struct VSInput
{
    float3 Position : POSITION;
    float2 Texcoord: TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 Texcoord: TEXCOORD;
};

VSOutput VSMain(VSInput Input)
{
    float4 Position = mul(float4(Input.Position.xyz, 1.0f), WorldViewProjectionTransform);

    VSOutput Result;
    Result.Position = Position;
    Result.Texcoord = Input.Texcoord;
    return Result;
}

