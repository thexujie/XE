#include "OverlappingCorners.h"

namespace fbx
{
    FOverlappingCorners::FOverlappingCorners(const std::vector<Float3> & InVertices, const std::vector<uint32_t> & InIndices, float ComparisonThreshold)
    {
        auto PointsEqual = [](const Float3 & V1, const Float3 & V2, float ComparisonThreshold)
        {
            if (std::abs(V1.X - V2.X) > ComparisonThreshold
                || std::abs(V1.Y - V2.Y) > ComparisonThreshold
                || std::abs(V1.Z - V2.Z) > ComparisonThreshold)
            {
                return false;
            }
            return true;
        };

        const int32_t NumWedges = InIndices.size();

        // Create a list of vertex Z/index pairs
        std::vector<IndexAndZ> VertIndexAndZ;
        VertIndexAndZ.resize(NumWedges);
        for (int32_t WedgeIndex = 0; WedgeIndex < NumWedges; WedgeIndex++)
        {
            Float3 Pos = InVertices[InIndices[WedgeIndex]];
            VertIndexAndZ[WedgeIndex] = { WedgeIndex, 0.30f * Pos.X + 0.33f * Pos.Y + 0.37f * Pos.Z };
        }

        // Sort the vertices by z value
        std::sort(VertIndexAndZ.begin(), VertIndexAndZ.end());

        Init(NumWedges);

        // Search for duplicates, quickly!
        for (int32_t i = 0; i < VertIndexAndZ.size(); i++)
        {
            // only need to search forward, since we add pairs both ways
            for (int32_t j = i + 1; j < VertIndexAndZ.size(); j++)
            {
                if (std::abs(VertIndexAndZ[j].Z - VertIndexAndZ[i].Z) > ComparisonThreshold)
                    break; // can't be any more dups

                const Float3 & PositionA = InVertices[InIndices[VertIndexAndZ[i].Index]];
                const Float3 & PositionB = InVertices[InIndices[VertIndexAndZ[j].Index]];

                if (PointsEqual(PositionA, PositionB, ComparisonThreshold))
                {
                    Add(VertIndexAndZ[i].Index, VertIndexAndZ[j].Index);
                }
            }
        }

        FinishAdding();
    }

    void FOverlappingCorners::Init(int32_t NumIndices)
    {
        Arrays.clear();
        Sets.clear();
        bFinishedAdding = false;

        IndexBelongsTo.resize(NumIndices);
        std::memset(IndexBelongsTo.data(), 0xFF, NumIndices * sizeof(int32_t));
    }

    void FOverlappingCorners::Add(int32_t Key, int32_t Value)
    {
        assert(Key != Value);
        assert(bFinishedAdding == false);

        int32_t ContainerIndex = IndexBelongsTo[Key];
        if (ContainerIndex == FBX_INDEX_NONE)
        {
            ContainerIndex = Arrays.size();
            Arrays.push_back({});
            std::vector<int32_t> & Container = Arrays.back();
            Container.reserve(6);
            Container.push_back(Key);
            Container.push_back(Value);
            IndexBelongsTo[Key] = ContainerIndex;
            IndexBelongsTo[Value] = ContainerIndex;
        }
        else
        {
            IndexBelongsTo[Value] = ContainerIndex;

            std::vector<int32_t> & ArrayContainer = Arrays[ContainerIndex];
            if (ArrayContainer.size() == 1)
            {
                // Container is a set
                Sets[ArrayContainer.back()].insert(Value);
            }
            else
            {
                // Container is an array
                AddUnique(ArrayContainer, Value);

                // Change container into set when one vertex is shared by large number of triangles
                if (ArrayContainer.size() > 12)
                {
                    int32_t SetIndex = Sets.size();
                    Sets.push_back({});
                    std::set<int32_t> & Set = Sets.back();

                    for (auto item : ArrayContainer)
                        Set.insert(item);

                    // Having one element means we are using a set
                    // An array will never have just 1 element normally because we add them as pairs
                    ArrayContainer.clear();
                    ArrayContainer.push_back(SetIndex);
                }
            }
        }
    }

    void FOverlappingCorners::FinishAdding()
    {
        assert(bFinishedAdding == false);

        for (std::vector<int32_t> & Array : Arrays)
        {
            // Turn sets back into arrays for easier iteration code
            // Also reduces peak memory later in the import process
            if (Array.size() == 1)
            {
                std::set<int32_t> & Set = Sets[Array.back()];
                for (int32_t i : Set)
                {
                    Array.push_back(i);
                }
            }

            // Sort arrays now to avoid sort multiple times
            std::sort(Array.begin(), Array.end());
        }

        Sets.clear();

        bFinishedAdding = true;
    }

    uint32_t FOverlappingCorners::GetAllocatedSize(void) const
    {
        assert(false);
        return 0;
        //uint32 BaseMemoryAllocated = IndexBelongsTo.GetAllocatedSize() + Arrays.GetAllocatedSize() + Sets.GetAllocatedSize();

        //uint32 ArraysMemory = 0;
        //for (const TArray<int32_t> & ArrayIt : Arrays)
        //{
        //    ArraysMemory += ArrayIt.GetAllocatedSize();
        //}

        //uint32 SetsMemory = 0;
        //for (const TSet<int32_t> & SetsIt : Sets)
        //{
        //    SetsMemory += SetsIt.GetAllocatedSize();
        //}

        //return BaseMemoryAllocated + ArraysMemory + SetsMemory;
    }


}