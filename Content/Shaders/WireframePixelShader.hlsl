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

cbuffer PrimitiveParemeters
{
    float4x4 WorldTransform;
    float4x4 ViewProjectionTransform;
    float4x4 WorldViewProjectionTransform;
};

cbuffer MaterialParemeters
{
    float3 Color;
};

struct VSOutput
{
    float4 Potision : SV_POSITION;
};

float4 PSMain(VSOutput Input) : SV_TARGET
{
    return float4(1,0,0, 1);
}

