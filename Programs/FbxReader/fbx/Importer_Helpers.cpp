#include "Importer.h"

namespace fbx
{
    FbxAMatrix Importer::ComputeTotalMatrix(FbxNode * Node)
    {
        FbxAMatrix Geometry;
        FbxVector4 Translation, Rotation, Scaling;
        Translation = Node->GetGeometricTranslation(FbxNode::eSourcePivot);
        Rotation = Node->GetGeometricRotation(FbxNode::eSourcePivot);
        Scaling = Node->GetGeometricScaling(FbxNode::eSourcePivot);
        Geometry.SetT(Translation);
        Geometry.SetR(Rotation);
        Geometry.SetS(Scaling);

        //For Single Matrix situation, obtain transfrom matrix from eDESTINATION_SET, which include pivot offsets and pre/post rotations.
        FbxAMatrix & GlobalTransform = Scene->GetAnimationEvaluator()->GetNodeGlobalTransform(Node);

        //We can bake the pivot only if we don't transform the vertex to the absolute position
        if (!_options.bTransformVertexToAbsolute)
        {
            if (_options.bBakePivotInVertex)
            {
                FbxAMatrix PivotGeometry;
                FbxVector4 RotationPivot = Node->GetRotationPivot(FbxNode::eSourcePivot);
                FbxVector4 FullPivot;
                FullPivot[0] = -RotationPivot[0];
                FullPivot[1] = -RotationPivot[1];
                FullPivot[2] = -RotationPivot[2];
                PivotGeometry.SetT(FullPivot);
                Geometry = Geometry * PivotGeometry;
            }
            else
            {
                //No Wedge transform and no bake pivot, it will be the mesh as-is.
                Geometry.SetIdentity();
            }
        }
        //We must always add the geometric transform. Only Max use the geometric transform which is an offset to the local transform of the node
        FbxAMatrix TotalMatrix = _options.bTransformVertexToAbsolute ? GlobalTransform * Geometry : Geometry;

        return TotalMatrix;
    }

    FbxAMatrix Importer::ComputeSkeletalMeshTotalMatrix(FbxNode * Node, FbxNode * RootSkeletalNode)
    {
        if (_options.bImportScene && !_options.bTransformVertexToAbsolute && RootSkeletalNode != nullptr && RootSkeletalNode != Node)
        {
            FbxAMatrix GlobalTransform = Scene->GetAnimationEvaluator()->GetNodeGlobalTransform(Node);
            FbxAMatrix GlobalSkeletalMeshRootTransform = Scene->GetAnimationEvaluator()->GetNodeGlobalTransform(RootSkeletalNode);
            FbxAMatrix TotalMatrix = GlobalSkeletalMeshRootTransform.Inverse() * GlobalTransform;
            return TotalMatrix;
        }
        return ComputeTotalMatrix(Node);
    }
}
