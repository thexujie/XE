#include "Importer.h"

namespace fbx
{
    const auto MaxReferenceRate = 1920.0f;

    FbxVector4 TransformVector(const FbxQuaternion & Q, const FbxVector4 & V)
    {
        FbxAMatrix am;
        am.SetQ(Q);
        return am.MultT(V);
    }

    FbxAMatrix GetRelativeTransform(const FbxAMatrix & BaseTransform, const FbxAMatrix & TargetTransform)
    {
        FbxQuaternion QT0(0.5, 0, 0, 0.13);
        FbxQuaternion QT1(-0.5, 0, 0, -0.13);
        QT0.Normalize();
        QT1.Normalize();

        FbxVector4 VT(1, 2, 3, 0);
        FbxVector4 VTT0 = TransformVector(QT0, VT);
        FbxVector4 VTT1 = TransformVector(QT1, VT);


        FbxVector4 Other_Translation = BaseTransform.GetT();
        FbxQuaternion Other_Rotation = BaseTransform.GetQ();
        FbxVector4 Other_Scale3D = BaseTransform.GetS();

        FbxVector4 Translation = TargetTransform.GetT();
        FbxQuaternion Rotation = TargetTransform.GetQ();
        FbxVector4 Scale3D = TargetTransform.GetS();

        FbxVector4 VSafeScale3D = FbxVector4(1, 1, 1, 1) / Other_Scale3D;
        VSafeScale3D[3] = 0;

        FbxVector4 VScale3D = Scale3D * VSafeScale3D;

        //VQTranslation = (  ( T(A).X - T(B).X ),  ( T(A).Y - T(B).Y ), ( T(A).Z - T(B).Z), 0.f );
        FbxVector4 VQTranslation = Translation - Other_Translation;
        VQTranslation[3] = 0;

        // Inverse RotatedTranslation
        FbxQuaternion VInverseRot = Other_Rotation;
        VInverseRot.Inverse();
        FbxVector4 VR = TransformVector(VInverseRot, VQTranslation);

        //Translation = 1/S(B)
        FbxVector4 VTranslation = VR * VSafeScale3D;

        // Rotation = Q(B)(-1) * Q(A)	
        FbxQuaternion VRotation = VInverseRot * Rotation;
        VRotation = {-VRotation [0], -VRotation [1], -VRotation [2], -VRotation [3]};
        FbxAMatrix Result;
        Result.SetTQS(VTranslation, VRotation, VScale3D);
        return Result;
    }

    int32_t GetTimeSampleRate(const float DeltaTime)
    {
        float OriginalSampleRateDivider = 1.0f / DeltaTime;
        float SampleRateDivider = OriginalSampleRateDivider;
        float SampleRemainder = Fractional(SampleRateDivider);
        float Multiplier = 2.0f;
        float IntegerPrecision = std::min(std::max(FBX_KINDA_SMALL_NUMBER * SampleRateDivider, FBX_KINDA_SMALL_NUMBER), 0.1f); //The precision is limit between KINDA_SMALL_NUMBER and 0.1f
        while (!IsNearlyZero(SampleRemainder, IntegerPrecision) && !IsNearlyZero(SampleRemainder, 1.0f, IntegerPrecision))
        {
            SampleRateDivider = OriginalSampleRateDivider * Multiplier;
            SampleRemainder = Fractional(SampleRateDivider);
            if (SampleRateDivider > MaxReferenceRate)
            {
                SampleRateDivider = FBX_DEFAULT_SAMPLERATE;
                break;
            }
            Multiplier += 1.0f;
        }
        return std::min(RoundToInt(SampleRateDivider), RoundToInt(MaxReferenceRate));
    }

    int32_t GetAnimationCurveRate(FbxAnimCurve * CurrentCurve)
    {
        if (CurrentCurve == nullptr)
            return 0;

        int32_t KeyCount = CurrentCurve->KeyGetCount();

        FbxTimeSpan TimeInterval(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
        bool bValidTimeInterval = CurrentCurve->GetTimeInterval(TimeInterval);
        if (KeyCount > 1 && bValidTimeInterval)
        {
            double KeyAnimLength = TimeInterval.GetDuration().GetSecondDouble();
            if (KeyAnimLength != 0.0)
            {
                //////////////////////////////////////////////////////////////////////////
                // 1. Look if we have high frequency keys(resampling).

                //Basic sample rate is compute by dividing the KeyCount by the anim length. This is valid only if
                //all keys are time equidistant. But if we find a rate over DEFAULT_SAMPLERATE, we can estimate that
                //there is a constant frame rate between the key and simply return the rate.
                int32_t SampleRate = RoundToInt((KeyCount - 1) / KeyAnimLength);
                if (SampleRate >= FBX_DEFAULT_SAMPLERATE)
                {
                    //We import a curve with more then 30 keys per frame
                    return SampleRate;
                }

                //////////////////////////////////////////////////////////////////////////
                // 2. Compute the sample rate of every keys with there time. Use the
                //    least common multiplier to get a sample rate that go through all keys.

                SampleRate = 1;
                float OldKeyTime = 0.0f;
                std::set<int32_t> DeltaComputed;
                //Reserve some space
                const float KeyMultiplier = (1.0f / FBX_KINDA_SMALL_NUMBER);
                //Find also the smallest delta time between keys
                for (int32_t KeyIndex = 0; KeyIndex < KeyCount; ++KeyIndex)
                {
                    float KeyTime = (float)(CurrentCurve->KeyGet(KeyIndex).GetTime().GetSecondDouble());
                    //Collect the smallest delta time, there is no delta in case the first animation key time is negative
                    float Delta = (KeyTime < 0 && KeyIndex == 0) ? 0.0f : KeyTime - OldKeyTime;
                    //use the fractional part of the delta to have the delta between 0.0f and 1.0f
                    Delta = Fractional(Delta);
                    int32_t DeltaKey = RoundToInt(Delta * KeyMultiplier);
                    if (!IsNearlyZero(Delta, FBX_KINDA_SMALL_NUMBER) && !(DeltaComputed.find(DeltaKey) != DeltaComputed.end()))
                    {
                        int32_t ComputeSampleRate = GetTimeSampleRate(Delta);
                        DeltaComputed.insert(DeltaKey);
                        //Use the least common multiplier with the new delta entry
                        int32_t LeastCommonMultiplier_ = std::min(LeastCommonMultiplier(SampleRate, ComputeSampleRate), RoundToInt(MaxReferenceRate));
                        SampleRate = LeastCommonMultiplier != 0 ? LeastCommonMultiplier_ : std::max(RoundToInt(FBX_DEFAULT_SAMPLERATE), std::max(SampleRate, ComputeSampleRate));
                    }
                    OldKeyTime = KeyTime;
                }
                return SampleRate;
            }
        }

        return 0;
    }

    void GetNodeSampleRate(FbxNode * Node, FbxAnimLayer * AnimLayer, std::vector<int32_t> & NodeAnimSampleRates, bool bCurve, bool bBlendCurve)
    {
        if (bCurve)
        {
            const int32_t MaxElement = 9;
            FbxAnimCurve * Curves[MaxElement];

            Curves[0] = Node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
            Curves[1] = Node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
            Curves[2] = Node->LclTranslation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
            Curves[3] = Node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
            Curves[4] = Node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
            Curves[5] = Node->LclRotation.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
            Curves[6] = Node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
            Curves[7] = Node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
            Curves[8] = Node->LclScaling.GetCurve(AnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);


            for (int32_t CurveIndex = 0; CurveIndex < MaxElement; ++CurveIndex)
            {
                FbxAnimCurve * CurrentCurve = Curves[CurveIndex];
                if (CurrentCurve)
                {
                    int32_t CurveAnimRate = GetAnimationCurveRate(CurrentCurve);
                    if (CurveAnimRate != 0)
                    {
                        AddUnique(NodeAnimSampleRates, CurveAnimRate);
                    }
                }
            }
        }

        if (bBlendCurve)
        {
            FbxGeometry * Geometry = (FbxGeometry *)Node->GetNodeAttribute();
            if (Geometry)
            {
                int32_t BlendShapeDeformerCount = Geometry->GetDeformerCount(FbxDeformer::eBlendShape);
                for (int32_t BlendShapeIndex = 0; BlendShapeIndex < BlendShapeDeformerCount; ++BlendShapeIndex)
                {
                    FbxBlendShape * BlendShape = (FbxBlendShape *)Geometry->GetDeformer(BlendShapeIndex, FbxDeformer::eBlendShape);

                    int32_t BlendShapeChannelCount = BlendShape->GetBlendShapeChannelCount();
                    for (int32_t ChannelIndex = 0; ChannelIndex < BlendShapeChannelCount; ++ChannelIndex)
                    {
                        FbxBlendShapeChannel * Channel = BlendShape->GetBlendShapeChannel(ChannelIndex);

                        if (Channel)
                        {
                            FbxAnimCurve * CurrentCurve = Geometry->GetShapeChannel(BlendShapeIndex, ChannelIndex, AnimLayer);
                            if (CurrentCurve)
                            {
                                int32_t CurveAnimRate = GetAnimationCurveRate(CurrentCurve);
                                if (CurveAnimRate != 0)
                                {
                                    AddUnique(NodeAnimSampleRates, CurveAnimRate);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    fbx::error_e Importer::ImportBoneTrackers()
    {
        auto GetAnimationTimeSpan = [&](FbxNode * RootNode, FbxAnimStack * AnimStack)->FbxTimeSpan
        {
            FbxTimeSpan AnimTimeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
            bool bUseDefault = _options.AnimationLengthImportType == EFBXAnimationLengthImportType::ExportedTime || std::abs(OriginalFbxFramerate - FBX_KINDA_SMALL_NUMBER) <= FBX_SMALL_NUMBER;
            if (bUseDefault)
            {
                AnimTimeSpan = AnimStack->GetLocalTimeSpan();
            }
            else if (_options.AnimationLengthImportType == EFBXAnimationLengthImportType::AnimatedKey)
            {
                RootNode->GetAnimationInterval(AnimTimeSpan, AnimStack);
            }
            else // then it's range 
            {
                AnimTimeSpan = AnimStack->GetLocalTimeSpan();

                FbxTimeSpan AnimatedInterval(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
                RootNode->GetAnimationInterval(AnimatedInterval, AnimStack);

                // find the most range that covers by both method, that'll be used for clamping
                FbxTime StartTime = std::min<FbxTime>(AnimTimeSpan.GetStart(), AnimatedInterval.GetStart());
                FbxTime StopTime = std::max<FbxTime>(AnimTimeSpan.GetStop(), AnimatedInterval.GetStop());

                // make inclusive time between localtimespan and animation interval
                AnimTimeSpan.SetStart(StartTime);
                AnimTimeSpan.SetStop(StopTime);

                FbxTime EachFrame = FBXSDK_TIME_ONE_SECOND / OriginalFbxFramerate;
                int32_t StartFrame = StartTime.Get() / EachFrame.Get();
                int32_t StopFrame = StopTime.Get() / EachFrame.Get();
                if (StartFrame != StopFrame)
                {
                    FbxTime Duration = AnimTimeSpan.GetDuration();

                    int32_t AnimRangeX = fbx::clamp<int32_t>(_options.AnimationRangeX, StartFrame, StopFrame);
                    int32_t AnimRangeY = fbx::clamp<int32_t>(_options.AnimationRangeY, StartFrame, StopFrame);

                    FbxLongLong Interval = EachFrame.Get();

                    // now set new time
                    if (StartFrame != AnimRangeX)
                    {
                        FbxTime NewTime(AnimRangeX * Interval);
                        AnimTimeSpan.SetStart(NewTime);
                    }

                    if (StopFrame != AnimRangeY)
                    {
                        FbxTime NewTime(AnimRangeY * Interval);
                        AnimTimeSpan.SetStop(NewTime);
                    }
                }
            }

            return AnimTimeSpan;
        };

        auto GetMaxSampleRate = [&](std::vector<FbxNode *> & SortedLinks, std::vector<FbxNode *> & NodeArray)->int32_t
        {
            int32_t MaxStackResampleRate = 0;
            std::vector<int32_t> CurveAnimSampleRates;
            int32_t AnimStackCount = Scene->GetSrcObjectCount<FbxAnimStack>();
            for (int32_t AnimStackIndex = 0; AnimStackIndex < AnimStackCount; AnimStackIndex++)
            {
                FbxAnimStack * CurAnimStack = Scene->GetSrcObject<FbxAnimStack>(AnimStackIndex);

                FbxTimeSpan AnimStackTimeSpan = GetAnimationTimeSpan(SortedLinks[0], CurAnimStack);

                double AnimStackStart = AnimStackTimeSpan.GetStart().GetSecondDouble();
                double AnimStackStop = AnimStackTimeSpan.GetStop().GetSecondDouble();

                int32_t AnimStackLayerCount = CurAnimStack->GetMemberCount();
                for (int32_t LayerIndex = 0; LayerIndex < AnimStackLayerCount; ++LayerIndex)
                {
                    FbxAnimLayer * AnimLayer = (FbxAnimLayer *)CurAnimStack->GetMember(LayerIndex);
                    for (int32_t LinkIndex = 0; LinkIndex < SortedLinks.size(); ++LinkIndex)
                    {
                        FbxNode * CurrentLink = SortedLinks[LinkIndex];
                        GetNodeSampleRate(CurrentLink, AnimLayer, CurveAnimSampleRates, true, false);
                    }

                    // it doens't matter whether you choose to import morphtarget or not
                    // blendshape are always imported. Import morphtarget is only used for morphtarget for mesh
                    {
                        for (int32_t NodeIndex = 0; NodeIndex < NodeArray.size(); NodeIndex++)
                        {
                            // consider blendshape animation curve
                            GetNodeSampleRate(NodeArray[NodeIndex], AnimLayer, CurveAnimSampleRates, false, true);
                        }
                    }
                }
            }

            MaxStackResampleRate = CurveAnimSampleRates.size() > 0 ? 1 : MaxStackResampleRate;
            //Find the lowest sample rate that will pass by all the keys from all curves
            for (int32_t CurveSampleRate : CurveAnimSampleRates)
            {
                if (CurveSampleRate >= MaxReferenceRate && MaxStackResampleRate < CurveSampleRate)
                {
                    MaxStackResampleRate = CurveSampleRate;
                }
                else if (MaxStackResampleRate < MaxReferenceRate)
                {
                    int32_t LeastCommonMultiplier_ = LeastCommonMultiplier(MaxStackResampleRate, CurveSampleRate);
                    MaxStackResampleRate = LeastCommonMultiplier_ != 0 ? LeastCommonMultiplier_ : std::max(RoundToInt(FBX_DEFAULT_SAMPLERATE), std::max(MaxStackResampleRate, CurveSampleRate));
                    if (MaxStackResampleRate >= MaxReferenceRate)
                    {
                        MaxStackResampleRate = MaxReferenceRate;
                    }
                }
            }

            // Make sure we're not hitting 0 for samplerate
            if (MaxStackResampleRate != 0)
            {
                assert(MaxStackResampleRate >= 0);
                //Make sure the resample rate is positive
                if (!(MaxStackResampleRate >= 0))
                {
                    MaxStackResampleRate *= -1;
                }
                return MaxStackResampleRate;
            }

            return FBX_DEFAULT_SAMPLERATE;
        };

    	// 对于纯动画文件来说，没有网格，所以也无法获取到根变换，忽略掉即可
        FbxNode * SkeletalMeshRootNode = _meshs.empty() ? nullptr : _meshs[0];

        size_t TotalNumKeys = 0;
        bool bIsRigidMeshAnimation = false;
        if (_options.bImportScene && _links.size() > 0)
        {
            for (size_t BoneIdx = 0; BoneIdx < _links.size(); ++BoneIdx)
            {
                FbxNode * Link = _links[BoneIdx];
                if (Link->GetMesh() && Link->GetMesh()->GetDeformerCount(FbxDeformer::eSkin) == 0)
                {
                    bIsRigidMeshAnimation = true;
                    break;
                }
            }
        }

        int32_t ResampleRate = FBX_DEFAULT_SAMPLERATE;
        if (_options.bResample)
        {
            if (_options.ResampleRate > 0)
            {
                ResampleRate = _options.ResampleRate;
            }
            else
            {
                // For FBX data, "Frame Rate" is just the speed at which the animation is played back.  It can change
                // arbitrarily, and the underlying data can stay the same.  What we really want here is the Sampling Rate,
                // ie: the number of animation keys per second.  These are the individual animation curve keys
                // on the FBX nodes of the skeleton.  So we loop through the nodes of the skeleton and find the maximum number 
                // of keys that any node has, then divide this by the total length (in seconds) of the animation to find the 
                // sampling rate of this set of data 

                // we want the maximum resample rate, so that we don't lose any precision of fast anims,
                // and don't mind creating lerped frames for slow anims
                int32_t BestResampleRate = GetMaxSampleRate(_links, _meshs);

                if (BestResampleRate > 0)
                {
                    ResampleRate = BestResampleRate;
                }
            }
        }

        FbxAMatrix AddedMatrix = _transform;
        //Set the time increment from the re-sample rate
        FbxTime TimeIncrement = 0;
        TimeIncrement.SetSecondDouble(1.0 / ((double)(ResampleRate)));
		_animSampleRate = ResampleRate;
    	
        //Add a threshold when we compare if we have reach the end of the animation
        const FbxTime TimeComparisonThreshold = (FBX_KINDA_SMALL_NUMBER * FBXSDK_TC_SECOND);

        int32_t AnimStackCount = Scene->GetSrcObjectCount<FbxAnimStack>();
        for (int32_t AnimStackIndex = 0; AnimStackIndex < AnimStackCount; AnimStackIndex++)
        {
            FbxAnimStack * CurAnimStack = Scene->GetSrcObject<FbxAnimStack>(AnimStackIndex);
            std::string animName = CurAnimStack->GetName();
			//int LayerCount = CurAnimStack->GetMemberCount<FbxAnimLayer>();
			//for (int j = 0; j < LayerCount; ++j)
			//{
			//	FbxAnimLayer* pLayer = CurAnimStack->GetMember<FbxAnimLayer>(j);
			//	const char* szLayerName = pLayer->GetName();
			//	szLayerName = 0;
			//	//ParseLayer(pNode, pLayer);
			//}
        	
            if (animName.empty())
                animName = fbx::format("AnimStack_", AnimStackIndex);
            FbxTimeSpan AnimTimeSpan = GetAnimationTimeSpan(_links[0], CurAnimStack);

            _anims.push_back({});
            Anim & anim = _anims.back();
            anim.name = animName;
            anim.duration = AnimTimeSpan.GetDuration();

            for (size_t ilink = 0; ilink < _links.size(); ++ilink)
            {
                anim.boneKeys.push_back({});
                auto & boneKey = anim.boneKeys.back();
                boneKey.boneName = _bones[ilink].Name;

                int BoneTreeIndex = ilink;
                FbxNode * Link = _links[ilink];
                FbxNode * LinkParent = Link->GetParent();
                for (FbxTime CurTime = AnimTimeSpan.GetStart(); CurTime < (AnimTimeSpan.GetStop() + TimeComparisonThreshold); CurTime += TimeIncrement)
                {
                    // save global trasnform
                    FbxAMatrix GlobalMatrix = Link->EvaluateGlobalTransform(CurTime) * _converter.GetJointPostConversionMatrix();
                    // we'd like to verify this before going to Transform. 
                    // currently transform has tons of NaN check, so it will crash there
                    ////Matrix GlobalUEMatrix = _converter.ConvertMatrix(GlobalMatrix);
                    ////if (GlobalUEMatrix.ContainsNaN())
                    ////{
                    ////    bSuccess = false;
                    ////    AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, FText::Format(LOCTEXT("Error_InvalidTransform",
                    ////        "Track {0} contains invalid transform. Could not import the track."), FText::FromName(BoneName))), FFbxErrors::Animation_TransformError);
                    ////    break;
                    ////}

                    ////if (GlobalTransform.ContainsNaN())
                    ////{
                    ////    bSuccess = false;
                    ////    AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, FText::Format(LOCTEXT("Error_InvalidUnrealTransform",
                    ////        "Track {0} has invalid transform(NaN). Zero scale transform can cause this issue."), FText::FromName(BoneName))), FFbxErrors::Animation_TransformError);
                    ////    break;
                    ////}

                    // debug data, including import transformation

                    FbxAMatrix LocalTransform;
                    if (!_options.bPreserveLocalTransform && LinkParent)
                    {
                        // I can't rely on LocalMatrix. I need to recalculate quaternion/scale based on global transform if Parent exists
                        FbxAMatrix ParentGlobalMatrix = LinkParent->EvaluateGlobalTransform(CurTime);
                        if (BoneTreeIndex != 0)
                        {
                            ParentGlobalMatrix = ParentGlobalMatrix * _converter.GetJointPostConversionMatrix();
                        }
                        //In case we do a scene import we need to add the skeletal mesh root node matrix to the parent link.
                        if (_options.bImportScene && !_options.bTransformVertexToAbsolute && BoneTreeIndex == 0 && SkeletalMeshRootNode != nullptr)
                        {
                            //In the case of a rigidmesh animation we have to use the skeletalMeshRootNode position at zero since the mesh can be animate.
                            FbxAMatrix GlobalSkeletalNodeFbx = bIsRigidMeshAnimation ? SkeletalMeshRootNode->EvaluateGlobalTransform(0) : SkeletalMeshRootNode->EvaluateGlobalTransform(CurTime);
                            ParentGlobalMatrix = ParentGlobalMatrix * GlobalSkeletalNodeFbx;
                        }

                        //LocalTransform = GlobalTransform.GetRelativeTransform(ParentGlobalTransform);

						//LocalTransform = Link->EvaluateLocalTransform(CurTime) * _converter.GetJointPostConversionMatrix();
						//LocalTransform = GlobalMatrix * ParentGlobalMatrix.Inverse();
                        LocalTransform = GetRelativeTransform(ParentGlobalMatrix, GlobalMatrix);
                        auto LocalTransform2 = ParentGlobalMatrix.Inverse() * GlobalMatrix;

                        bool Equal = LocalTransform == LocalTransform2;
                        if (!Equal)
                            Equal = true;

                        auto LocalTransform_Calc = ParentGlobalMatrix.Inverse() * GlobalMatrix;
                        auto LocalTransform_Evaluate = Link->EvaluateLocalTransform(CurTime);
                        int EqualCalculate = LocalTransform.Compare(LocalTransform_Calc, 1e-03);
                        int EqualEvaluate = LocalTransform.Compare(LocalTransform_Evaluate, 1e-03);
                        assert(EqualCalculate == 0);
                        assert(EqualEvaluate == 0);
                    	
						//auto ParentGlobalMatrixT = ParentGlobalMatrix.GetT();
						//auto ParentGlobalMatrixQ = ParentGlobalMatrix.GetQ();
						//auto ParentGlobalMatrixR = ParentGlobalMatrix.GetR();
						//auto ParentGlobalMatrixS = ParentGlobalMatrix.GetS();
      //              	
						//auto GlobalMatrixT = GlobalMatrix.GetT();
						//auto GlobalMatrixQ = GlobalMatrix.GetQ();
						//auto GlobalMatrixR = GlobalMatrix.GetR();
						//auto GlobalMatrixS = GlobalMatrix.GetS();
      //              	
      //                  auto T9 = LocalTransform.GetT();
      //                  auto Q9 = LocalTransform.GetQ();
      //                  auto R9 = LocalTransform.GetR();
      //                  auto S9 = LocalTransform.GetS();
                    }
                    else
                    {
                        LocalTransform = Link->EvaluateLocalTransform(CurTime);
                    }

                    if ( BoneTreeIndex == 0)
                    {
                        // If we found template data earlier, apply the import transform matrix to
                        // the root track.
                        LocalTransform = LocalTransform * AddedMatrix;
                    }

                    ////if (LocalTransform.ContainsNaN())
                    ////{
                    ////    bSuccess = false;
                    ////    AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Error, FText::Format(LOCTEXT("Error_InvalidUnrealLocalTransform",
                    ////        "Track {0} has invalid transform(NaN). If you have zero scale transform, that can cause this."), FText::FromName(BoneName))), FFbxErrors::Animation_TransformError);
                    ////    break;
                    ////}

                    boneKey.keys.push_back(LocalTransform);
                }

                TotalNumKeys = std::max(TotalNumKeys, boneKey.keys.size());
            }
        }
        return fbx::error_ok;
    }
}
