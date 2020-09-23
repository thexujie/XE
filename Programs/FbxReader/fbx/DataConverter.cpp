#include "DataConverter.h"

namespace fbx
{
    DataConverter::DataConverter()
    {
        
    }

    DataConverter::~DataConverter()
    {
        
    }

    Float3 DataConverter::ConvertPos(FbxVector4 Vector)
    {
        Float3 Out;
        Out[0] = Vector[0];
        // flip Y, then the right-handed axis system is converted to LHS
        Out[1] = Vector[1];
        Out[2] = Vector[2];
        return Out;
    }



    Float3 DataConverter::ConvertDir(FbxVector4 Vector)
    {
        Float3 Out;
        Out[0] = Vector[0];
        Out[1] = Vector[1];
        Out[2] = Vector[2];
        return Out;
    }

    FbxDouble3 DataConverter::ConvertEuler(FbxDouble3 Euler)
    {
        //return FbxDouble3::MakeFromEuler(FbxVector4(Euler[0], -Euler[1], Euler[2]));
        return Euler;
    }


    Float3 DataConverter::ConvertScale(FbxDouble3 Vector)
    {
        Float3 Out;
        Out[0] = Vector[0];
        Out[1] = Vector[1];
        Out[2] = Vector[2];
        return Out;
    }


    Float3 DataConverter::ConvertScale(FbxVector4 Vector)
    {
        Float3 Out;
        Out[0] = Vector[0];
        Out[1] = Vector[1];
        Out[2] = Vector[2];
        return Out;
    }

    Quat DataConverter::ConvertRotation(FbxQuaternion Quaternion)
    {
        Quat Out(ConvertRotToQuat(Quaternion));
        return Out;
    }

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    FbxVector4 DataConverter::ConvertRotationToFVect(FbxQuaternion Quaternion, bool bInvertOrient)
    {
        assert(false);
        return {};

        //FbxQuaternion UnrealQuaternion = ConvertRotToQuat(Quaternion);
        //FbxVector4 Euler;
        //Euler = UnrealQuaternion.Euler();
        //if (bInvertOrient)
        //{
        //    Euler.Y = -Euler.Y;
        //    Euler.Z = 180.f + Euler.Z;
        //}
        //return Euler;
    }

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    Quat DataConverter::ConvertRotToQuat(FbxQuaternion Quaternion)
    {
        Quat UnrealQuat;
        UnrealQuat[0] = Quaternion[0];
        UnrealQuat[1] = Quaternion[1];
        UnrealQuat[2] = Quaternion[2];
        UnrealQuat[3] = Quaternion[3];

        return UnrealQuat;
    }

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    float DataConverter::ConvertDist(FbxDouble Distance)
    {
        float Out;
        Out = (float)Distance;
        return Out;
    }

    Transform DataConverter::ConvertTransform(FbxAMatrix Matrix)
    {
        Transform Out;
        Out.Translation = ConvertPos(Matrix.GetT());
        Out.Rotation = ConvertRotToQuat(Matrix.GetQ());
        Out.Scale = ConvertScale(Matrix.GetS());
        return Out;
    }

    FbxAMatrix DataConverter::ConvertTransform(const Transform & transform)
    {
        return {};
    }

    FbxAMatrix DataConverter::ConvertMatrix(FbxAMatrix Matrix)
    {
        FbxAMatrix UEMatrix;

        for (int i = 0; i < 4; ++i)
        {
            FbxVector4 Row = Matrix.GetRow(i);
            if (i == 1)
            {
                UEMatrix[i][0] = Row[0];
                UEMatrix[i][1] = Row[1];
                UEMatrix[i][2] = Row[2];
                UEMatrix[i][3] = Row[3];
            }
            else
            {
                UEMatrix[i][0] = Row[0];
                UEMatrix[i][1] = Row[1];
                UEMatrix[i][2] = Row[2];
                UEMatrix[i][3] = Row[3];
            }
        }

        return UEMatrix;
    }

    FbxDouble3 DataConverter::ConvertColor(FbxDouble3 Color)
    {
        assert(false);
        return Color;
        //Fbx is in linear color space
        //FbxDouble3 SRGBColor = FLinearColor(Color[0], Color[1], Color[2]).ToFColor(true);
        //return SRGBColor;
    }

    FbxVector4 DataConverter::ConvertToFbxPos(FbxVector4 Vector)
    {
        FbxVector4 Out;
        Out[0] = Vector[0];
        Out[1] = -Vector[1];
        Out[2] = Vector[2];

        return Out;
    }

    FbxVector4 DataConverter::ConvertToFbxRot(FbxVector4 Vector)
    {
        FbxVector4 Out;
        Out[0] = Vector[0];
        Out[1] = Vector[1];
        Out[2] = Vector[2];

        return Out;
    }

    FbxVector4 DataConverter::ConvertToFbxScale(FbxVector4 Vector)
    {
        FbxVector4 Out;
        Out[0] = Vector[0];
        Out[1] = Vector[1];
        Out[2] = Vector[2];

        return Out;
    }

    FbxDouble3 DataConverter::ConvertToFbxColor(FbxDouble3 Color)
    {
        return Color;
        //Fbx is in linear color space
        //FLinearColor FbxLinearColor(Color);
        //FbxDouble3 Out;
        //Out[0] = FbxLinearColor.R;
        //Out[1] = FbxLinearColor.G;
        //Out[2] = FbxLinearColor.B;

        //return Out;
    }
}