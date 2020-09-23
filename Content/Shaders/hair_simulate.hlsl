

#define MAX_COLLISION_SPHERES 10

struct Vertex
{
    float4 position;
    float2 uv;
};

cbuffer SimulateConstantBuffer : register(b0)
{
    row_major float4x4 matrWorld;

    float gravityAcceleration;
    float gravityStrength;
    float timeElapse;
    float angularStiffness;

    // 约束计算的积分次数，越多越精确
    uint numConstraintIterations;
    float3 eyePos;

    // 碰撞球数量
    uint numSphereImplicits;
    uint3 _unused;
    
    row_major float4x4 CollisionSphereTransformations[MAX_COLLISION_SPHERES];
    row_major float4x4 CollisionSphereInverseTransformations[MAX_COLLISION_SPHERES];
};

Buffer<uint> strandOffsets : register(t0);
// liner length / line stiffness / anguar length/ angular stiffnessr
Buffer<float4> constraints : register(t1);
Buffer<float4> initPositions : register(t2);

RWBuffer<float4> currPositions : register(u0);
RWBuffer<float4> prevPositions : register(u1);

#define BLOCK_SIZE 64
groupshared float4 sharedPos[BLOCK_SIZE];
groupshared float4 sharedConstraints[BLOCK_SIZE];
groupshared float3 sharedForce[BLOCK_SIZE];

float3 SphereConstraint(float4 position, int isphere)
{
    if (position.w == 0)
        return 0;

    //inverse transform the position
    float3 transformedPosition = (mul(float4(position.xyz, 1), CollisionSphereInverseTransformations[isphere])).xyz;
    
    //find the force
    float3 force = float3(0, 0, 0);
    float len = length(transformedPosition);
    
    if (len < 1)
    {
        force = (1 - len) * normalize(transformedPosition);
    
        float4x4 sphereTransform = CollisionSphereTransformations[isphere];
        force = (mul(float4(force, 0), sphereTransform)).xyz;
    }

    return force;
}

void addObstacleCollisionResponse(inout float4 position)
{
    //spheres
    for (uint isphere = 0; isphere < numSphereImplicits; isphere++)
    {
        position.xyz += SphereConstraint(position, isphere);
    }
}

float4 addForcesAndIntegrate(float4 position, float4 oldPosition, float3 force, float stiffness, int globalIndex)
{
    float3 transformedPos = mul(float4(initPositions[globalIndex].xyz, 1), matrWorld).xyz;
    // 根控制点保持固定
    if (position.w == 0)
        return float4(transformedPos, position.w);

    float3 velocity = (position.xyz - oldPosition.xyz) * lerp(0.95f, 0.98f, stiffness);
    // 低速时加大刚度
    float stiffness0 = stiffness;
    //if (length(velocity) < 0.01)
    //{
    //    stiffness = saturate(stiffness * 5.0);
    //    if (stiffness - stiffness0 > 0)
    //        stiffness0 *= (stiffness - stiffness0) * 6;
    //}
    //else if (length(velocity) > 0.5f)
    //{
    //    stiffness *= 0.5f;
    //    stiffness0 *= 0.5f;
    //}
    //else {}

    force += float3(0.0f, -gravityAcceleration * lerp(1.0f, 0.5f, stiffness), 0.0f);
    float3 result = position.xyz + velocity + force * timeElapse * timeElapse / 20.0f * 400.0f;
    float staticky = stiffness0 * 0.0475;
    if (length(result - transformedPos) < gravityAcceleration)
        result = lerp(result, transformedPos, staticky);
    return float4(result, position.w);
}

bool IsFree(float4 particle)
{
    return particle.w > 0;
}

float2 Responsiveness(float4 particle0, float4 particle1)
{
    if (IsFree(particle0))
    {
        if (IsFree(particle1))
            return float2(0.5, 0.5);
        else
            return float2(1, 0);
    }
    else
    {
        if (IsFree(particle1))
            return float2(0, 1);
        else
            return float2(0, 0);
    }
}

void DistanceConstraint2(inout float4 particle0, inout float4 particle1, float2 constrain)
{
    float3 delta = particle1.xyz - particle0.xyz;
    float distance = max(length(delta), 1e-7);
    float stretching = 1 - constrain.x / distance;
    delta = stretching * delta;
    float2 responsiveness = Responsiveness(particle0, particle1);
    particle0.xyz += responsiveness[0] * delta * constrain.y;
    particle1.xyz -= responsiveness[1] * delta * constrain.y;
}


void DistanceConstraint(inout float4 particle0, inout float4 particle1, float targetDistance, float stiffness = 1.0)
{
    float3 delta = particle1.xyz - particle0.xyz;
    float distance = max(length(delta), 1e-7);
    float stretching = 1 - targetDistance / distance;
    delta = stretching * delta;
    float2 responsiveness = Responsiveness(particle0, particle1);
    particle0.xyz += responsiveness[0] * delta * stiffness;
    particle1.xyz -= responsiveness[1] * delta * stiffness;
}

[numthreads(BLOCK_SIZE, 1, 1)]
void CSMain(uint localIndex : SV_GroupIndex, uint3 groupId : SV_GroupID, uint3 dispatchId : SV_DispatchThreadID)
{
    uint indexBase = groupId.x > 0 ? strandOffsets[groupId.x - 1] : 0;
    uint globalIndex = indexBase + localIndex;
    uint count = strandOffsets[groupId.x] - indexBase;
    
    float4 orgPosition = float4(0, 0, 0, 0);
    if (localIndex < count)
    {
        sharedPos[localIndex] = currPositions[globalIndex];
        sharedConstraints[localIndex] = constraints[globalIndex];
        sharedForce[localIndex] = float3(0, 0, 0);
        orgPosition = currPositions[globalIndex];
    }
    GroupMemoryBarrierWithGroupSync();

    uint _half = count / 2;
    uint _half2 = (count - 1) / 2;
    uint _half3 = (count - 3) / 2;

    uint halfAng1 = max(3, _half2) - 3;
    uint halfAng2 = max(3, _half3) - 3;

    if (localIndex < count)
    {
        sharedPos[localIndex] = addForcesAndIntegrate(sharedPos[localIndex], prevPositions[globalIndex], sharedForce[localIndex], constraints[globalIndex].y, globalIndex);
        //sharedPos[localIndex].xyz = mul(float4(initPositions[globalIndex].xyz, 1), transform).xyz;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    for (uint iteration = 0; iteration < numConstraintIterations; iteration++)
    {
        //apply distance constraints to first subset
        if (localIndex < _half)
            DistanceConstraint(sharedPos[localIndex * 2], sharedPos[localIndex * 2 + 1], sharedConstraints[localIndex * 2].x);
        
        GroupMemoryBarrierWithGroupSync();
        
        //apply distance constraints to second subset
        if (localIndex < _half2)
            DistanceConstraint(sharedPos[localIndex * 2 + 1], sharedPos[localIndex * 2 + 2], sharedConstraints[localIndex * 2 + 1].x);
        
        GroupMemoryBarrierWithGroupSync();

        int idM2 = localIndex % 2;
        int idD2 = localIndex / 2;
        int id = idD2 * 4 + idM2;

        //apply the angular constraints to the first subset
        if (localIndex < halfAng1)
        {
            float stiffness = sharedConstraints[id].w * angularStiffness;
            if (stiffness > 0)
                DistanceConstraint(sharedPos[id], sharedPos[id + 2], sharedConstraints[id].z, stiffness);
        }
        GroupMemoryBarrierWithGroupSync();

        //apply the angular constraints to the second subset
        if (localIndex < halfAng2)
        {
            float stiffness = sharedConstraints[id + 2].w * angularStiffness;
            if (stiffness > 0)
                DistanceConstraint(sharedPos[id + 2], sharedPos[id + 4], sharedConstraints[id + 2].z, stiffness);
        }
        GroupMemoryBarrierWithGroupSync();

        addObstacleCollisionResponse(sharedPos[localIndex]);
        GroupMemoryBarrierWithGroupSync();
    }
    
    if (localIndex < count)
    {
        currPositions[globalIndex] = sharedPos[localIndex];
        prevPositions[globalIndex] = orgPosition;
    }
}

