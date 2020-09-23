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
//
#define RootBufferIndex_VertexFactory b4
#define RootBufferIndex_Material b5
#define RootBufferIndex_Primitive b6

#define RootSamplerIndex_Linear s0
#define RootSamplerIndex_Point s1


#define RootDescriptorTableIndex_Material t4

SamplerState PointWrappedSampler : register(s10);
SamplerState PointClampedSampler : register(s11);
SamplerState BilinearWrappedSampler : register(s12);
SamplerState BilinearClampedSampler : register(s13);
SamplerState TrilinearWrappedSampler : register(s14);
SamplerState TrilinearClampedSampler : register(s15);
