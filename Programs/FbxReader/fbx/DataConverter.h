#pragma once

#include "Fbx.h"

namespace fbx
{
    class DataConverter
    {
    public:
        DataConverter();
        ~DataConverter();

    public:
       void SetJointPostConversionMatrix(FbxAMatrix ConversionMatrix) { JointPostConversionMatrix = ConversionMatrix; }
       const FbxAMatrix & GetJointPostConversionMatrix() { return JointPostConversionMatrix; }

       void SetAxisConversionMatrix(FbxAMatrix ConversionMatrix) { AxisConversionMatrix = ConversionMatrix; AxisConversionMatrixInv = ConversionMatrix.Inverse(); }
       const FbxAMatrix & GetAxisConversionMatrix() { return AxisConversionMatrix; }
       const FbxAMatrix & GetAxisConversionMatrixInv() { return AxisConversionMatrixInv; }

       Float3 ConvertPos(FbxVector4 Vector);
       Float3 ConvertDir(FbxVector4 Vector);
       FbxDouble3 ConvertEuler(FbxDouble3 Euler);
       Float3 ConvertScale(FbxDouble3 Vector);
       Float3 ConvertScale(FbxVector4 Vector);
       Quat ConvertRotation(FbxQuaternion Quaternion);
       FbxVector4 ConvertRotationToFVect(FbxQuaternion Quaternion, bool bInvertRot);
       Quat ConvertRotToQuat(FbxQuaternion Quaternion);
       float ConvertDist(FbxDouble Distance);
       Transform ConvertTransform(FbxAMatrix Matrix);
       FbxAMatrix ConvertTransform(const Transform & transform);
       FbxAMatrix ConvertMatrix(FbxAMatrix Matrix);

        /*
         * Convert fbx linear space color to sRGB FbxDouble3
         */
       FbxDouble3 ConvertColor(FbxDouble3 Color);

       FbxVector4 ConvertToFbxPos(FbxVector4 Vector);
       FbxVector4 ConvertToFbxRot(FbxVector4 Vector);
       FbxVector4 ConvertToFbxScale(FbxVector4 Vector);

        /*
        * Convert sRGB FbxDouble3 to fbx linear space color
        */
       FbxDouble3   ConvertToFbxColor(FbxDouble3 Color);

        // FbxCamera with no rotation faces X with Y-up while ours faces X with Z-up so add a -90 degrees roll to compensate
       FbxDouble3 GetCameraRotation() { return FbxDouble3(0.f, 0.f, -90.f); }

        // FbxLight with no rotation faces -Z while ours faces Y so add a 90 degrees pitch to compensate
       FbxDouble3 GetLightRotation() { return FbxDouble3(0.f, 90.f, 0.f); }

    private:
       FbxAMatrix JointPostConversionMatrix;
       FbxAMatrix AxisConversionMatrix;
       FbxAMatrix AxisConversionMatrixInv;
    };
}