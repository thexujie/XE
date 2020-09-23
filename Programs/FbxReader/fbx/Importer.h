#pragma once

#include "Fbx.h"

#include "DataConverter.h"
#include "OverlappingCorners.h"
#include "FbxModel.h"

namespace fbx
{
    enum class EVertexColorImportOption
    {
        /** Import the static mesh using the vertex colors from the FBX file. */
        Replace = 0,
        /** Ignore vertex colors from the FBX file, and keep the existing mesh vertex colors. */
        Ignore,
        /** Override all vertex colors with the specified color. */
        Override
    };
    enum class EFBXNormalImportMethod
    {
        ComputeNormals = 0,
        ImportNormals,
        ImportNormalsAndTangents,
        MAX,
    };

    enum class EFBXAnimationLengthImportType
    {
        /** This option imports animation frames based on what is defined at the time of export */
        ExportedTime = 0,
        /** Will import the range of frames that have animation. Can be useful if the exported range is longer than the actual animation in the FBX file */
        AnimatedKey,
        /** This will enable the Start Frame and End Frame properties for you to define the frames of animation to import */
        SetRange,

        MAX,
    };

    enum class EFBXNormalGenerationMethod
    {
        /** Use the legacy built in method to generate normals (faster in some cases) */
        BuiltIn = 0,
        /** Use MikkTSpace to generate normals and tangents */
        MikkTSpace,
    };

    struct ImportThresholds
    {
    public:
        ImportThresholds()
            : ThresholdPosition(FBX_THRESH_POINTS_ARE_SAME)
            , ThresholdTangentNormal(FBX_THRESH_NORMALS_ARE_SAME)
            , ThresholdUV(FBX_THRESH_UVS_ARE_SAME)
        {}

        /** Threshold use to decide if two vertex position are equal. */
        float ThresholdPosition;

        /** Threshold use to decide if two normal, tangents or bi-normals are equal. */
        float ThresholdTangentNormal;

        /** Threshold use to decide if two UVs are equal. */
        float ThresholdUV;
    };

    struct ImportOptions
    {
        // 是导入场景还是导入模型
        const bool bImportScene = false;
        bool bConvertScene = true;
        bool bForceFrontXAxis = false;
        bool bConvertSceneUnit = false;
        bool bUseT0AsRefPose = false;
        bool bPreserveSmoothingGroups = true;
        EVertexColorImportOption VertexColorImportOption = EVertexColorImportOption::Replace;
        Color32 VertexOverrideColor;

        bool bBakePivotInVertex = false;
        bool bTransformVertexToAbsolute = true;

        bool bImportMorph = true;

        bool bUseMikkTSpace = true;
        bool bComputeNormals = true;
        bool bComputeTangents = true;
        bool bRemoveDegenerateTriangles = false;

        EFBXNormalImportMethod NormalImportMethod = EFBXNormalImportMethod::ComputeNormals;
        EFBXNormalGenerationMethod NormalGenerationMethod = EFBXNormalGenerationMethod::MikkTSpace;
        EFBXAnimationLengthImportType AnimationLengthImportType = EFBXAnimationLengthImportType::ExportedTime;

        // 将骨架导入为几何体
        bool bImportAsSkeletalGeometry = false;

        ImportThresholds OverlappingThresholds;

        bool bResample = true;
        int ResampleRate = 0;
        int AnimationRangeX = 0;
        int AnimationRangeY = 0;
        bool bPreserveLocalTransform = false;

        bool ShouldImportNormals() const
        {
            return NormalImportMethod == EFBXNormalImportMethod::ImportNormals || NormalImportMethod == EFBXNormalImportMethod::ImportNormalsAndTangents;
        }

        bool ShouldImportTangents() const
        {
            return NormalImportMethod == EFBXNormalImportMethod::ImportNormalsAndTangents;
        }

        void ResetForReimportAnimation()
        {
            bImportMorph = true;
            AnimationLengthImportType = EFBXAnimationLengthImportType::ExportedTime;
        }
    };

    class Importer
    {
    public:
        Importer();
        ~Importer();

        fbx::error_e LoadFromFile(std::string path);

    private:
        fbx::error_e ConvertScene();
        fbx::error_e ImportBone();
        fbx::error_e ImportMaterials();
        fbx::error_e ImportMeshs();
        fbx::error_e BuildSections();
        fbx::error_e BuildModel();
        fbx::error_e ImportMorphTargets();
        fbx::error_e ImportBoneTrackers();

    private:
        fbx::error_e importMesh(FbxMesh * Mesh, FbxSkin * Skin, FbxShape * FbxShape, FbxNode * RootNode);
        fbx::error_e importMeshPoints(std::vector<Float3> & points, FbxGeometryBase * geometry, const FbxAMatrix & amatrix);
        // 将姿态应用到顶点
        fbx::error_e skinControlPointsToPose(std::vector<Float3> & points, size_t pointIndexBase, FbxMesh * Mesh, FbxShape * FbxShape, bool bUseT0);
        fbx::error_e unsmoothVerts(bool bDuplicateUnSmoothWedges = true);
        fbx::error_e processInfluences();

    private: // Helpers
        FbxAMatrix ComputeSkeletalMeshTotalMatrix(FbxNode * Node, FbxNode * RootSkeletalNode);
        FbxAMatrix ComputeTotalMatrix(FbxNode * Node);

    private: // Calc
        fbx::error_e ComputeTangents_MikkTSpace();
        fbx::error_e ComputeTangents();


    public: // Utils
        static bool IsUnrealBone(FbxNode * link);
        static FbxAMatrix ComputeTotalMatrix(FbxScene * scene, FbxNode * node, bool transformToAbs = false, bool bakePivotInVertex = false);
        static FbxNode * GetRootSkeleton(FbxScene * scene, FbxNode * link, EFbxCreator creator = EFbxCreator::Unknow);
        static std::vector<FbxPose *> RetrievePoseFromBindPose(FbxScene * scene, std::vector<FbxNode *> nodes);
        static bool IsOddNegativeScale(FbxAMatrix & TotalMatrix);
        static BoundingBox CalcBoundingBox(const Float3 * points, size_t count);
        static inline bool PointsEqual(const Float3 & V1, const Float3 & V2, bool bUseEpsilonCompare = true)
        {
            const float Epsilon = bUseEpsilonCompare ? FBX_THRESH_POINTS_ARE_SAME : 0.0f;
            return std::abs(V1.X - V2.X) <= Epsilon && std::abs(V1.Y - V2.Y) <= Epsilon && std::abs(V1.Z - V2.Z) <= Epsilon;
        }

        static inline bool PointsEqual(const Float3 & V1, const Float3 & V2, const ImportThresholds & Threshold)
        {
            const float Epsilon = Threshold.ThresholdPosition;
            return std::abs(V1.X - V2.X) <= Epsilon && std::abs(V1.Y - V2.Y) <= Epsilon && std::abs(V1.Z - V2.Z) <= Epsilon;
        }
        static inline bool NormalsEqual(const Float3 & V1, const Float3 & V2)
        {
            const float Epsilon = FBX_THRESH_NORMALS_ARE_SAME;
            return std::abs(V1.X - V2.X) <= Epsilon && std::abs(V1.Y - V2.Y) <= Epsilon && std::abs(V1.Z - V2.Z) <= Epsilon;
        }
        static inline bool NormalsEqual(const Float3 & V1, const Float3 & V2, const ImportThresholds & Threshold)
        {
            const float Epsilon = Threshold.ThresholdTangentNormal;
            return std::abs(V1.X - V2.X) <= Epsilon && std::abs(V1.Y - V2.Y) <= Epsilon && std::abs(V1.Z - V2.Z) <= Epsilon;
        }

        static inline bool UVsEqual(const Float3 & V1, const Float3 & V2)
        {
            const float Epsilon = FBX_THRESH_UVS_ARE_SAME;
            return std::abs(V1.X - V2.X) <= Epsilon && std::abs(V1.Y - V2.Y) <= Epsilon && std::abs(V1.Z - V2.Z) <= Epsilon;
        }
        static inline bool UVsEqual(const Float2 & V1, const Float2 & V2, const ImportThresholds & Threshold)
        {
            const float Epsilon = Threshold.ThresholdUV;
            return std::abs(V1.X - V2.X) <= Epsilon && std::abs(V1.Y - V2.Y) <= Epsilon;
        }

        static bool AreSkelMeshVerticesEqual(const Vertex & V1, const Vertex & V2, const ImportThresholds & Threshold);
        static void ComputeTriangleTangents(const std::vector<Float3> & points, const std::vector<uint32_t> & indices, const std::vector<Float2> & uvs,
            std::vector<Float3> & tangentsX, std::vector<Float3> & tangentsY, std::vector<Float3> & tangentsZ, float ComparisonThreshold);
        static void ComputeNormals(const std::vector<Float3> & points, const std::vector<uint32_t> & indices, const std::vector<uint32_t> & groupIndices, const std::vector<Float2> & uvs, std::vector<Float3> & normals,
            bool ignoreDegenerateTriangles, bool blendOverlappingNormals);
    private:
        std::unique_ptr<FbxManager, void(*)(FbxManager *)> Manager = std::unique_ptr<FbxManager, void(*)(FbxManager *)>(nullptr, nullptr);
        std::unique_ptr<FbxScene, void(*)(FbxScene *)> Scene = std::unique_ptr<FbxScene, void(*)(FbxScene *)>(nullptr, nullptr);
        std::unique_ptr<FbxGeometryConverter> GeometryConverter = std::unique_ptr<FbxGeometryConverter>(nullptr);
        std::unique_ptr<FbxImporter, void(*)(FbxImporter *)> Imper = std::unique_ptr<FbxImporter, void(*)(FbxImporter *)>(nullptr, nullptr);

    public:
        const ImportOptions & Options() const { return _options; }
        const std::vector<Material> & Materials() const { return _materials; }
        const std::vector<Bone> & Bones() const { return _bones; }
        const std::vector<Float3> & Points() const { return _points; }
        const std::vector<Wedge> & Wedges() const { return _wedges; }
        const std::vector<Triangle> & Faces() const { return _faces; }
        const std::vector<BoneInfluence> & Influences() const { return _influences; }
        bool HasVertexColors() const { return _hasVertexColors; }
        const std::vector<Section> & Sections() const { return _sections; }
        const ModelData & Model() const { return _model; }
        const std::map<std::string, std::vector<MorphTargetDelta>> & MorphTargets() const { return _morphDeltas; }
		int32_t AnimSampleRate() const { return _animSampleRate; }
		const std::vector<Anim> & Anims() const { return _anims; }

    private:
        int _fileVersionMajor = 0;
        int _fileVersionMinor= 0;
        int _fileVersionRevision = 0;
        int _sdkVersionMajor = 0;
        int _sdkVersionMinor = 0;
        int _sdkVersionRevision = 0;
        EFbxCreator _creator = EFbxCreator::Unknow;
        ImportOptions _options;
        FbxAxisSystem _fileAxis;
        FbxSystemUnit _fileUnit;
        float OriginalFbxFramerate = 0.0f;
        FbxAMatrix _transform;

        DataConverter _converter;

		std::vector<FbxNode *> _nodes;
		std::vector<FbxNode *> _meshs;
        std::vector<FbxNode *> _skels;
        std::vector<FbxNode *> _links;

        std::vector<Bone> _bones;
        std::vector<Material> _materials;
        bool _useGeneratedBones = false;
        bool _useTime0AsRefPose = false;
        bool _bOutDiffPose = false;

        std::vector<Float3> _points;
        std::vector<int32_t> _pointToRawMap;
        std::vector<Wedge> _wedges;
        std::vector<Triangle> _faces;
        std::vector<BoneInfluence> _influences;

        FOverlappingCorners _overlappingCorners;
        std::vector<Float3> _tangentsX;
        std::vector<Float3> _tangentsY;
        std::vector<Float3> _tangentsZ;

        std::vector<Section> _sections;

        uint32_t _nTexCoords = 0;
        uint32_t _maxMaterialIndex = 0;
        bool _hasVertexColors = false;
        bool _hasNormals = false;
        bool _hasTangents = false;

        ModelData _model;
        std::map<std::string, std::vector<MorphTargetDelta>> _morphDeltas;

		int32_t _animSampleRate = 30;
        std::vector<Anim> _anims;
    };
}
