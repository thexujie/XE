#pragma once

#include "Fbx.h"

namespace fbx
{
    struct FOverlappingCorners
    {
        FOverlappingCorners() {}

        FOverlappingCorners(const std::vector<Float3> & InVertices, const std::vector<uint32_t> & InIndices, float ComparisonThreshold);

        /* Resets, pre-allocates memory, marks all indices as not overlapping in preperation for calls to Add() */
        void Init(int32_t NumIndices);

        /* Add overlapping indices pair */
        void Add(int32_t Key, int32_t Value);

        /* Sorts arrays, converts sets to arrays for sorting and to allow simple iterating code, prevents additional adding */
        void FinishAdding();

        /* Estimate memory allocated */
        uint32_t GetAllocatedSize(void) const;

        /**
        * @return array of sorted overlapping indices including input 'Key', empty array for indices that have no overlaps.
        */
        const std::vector<int32_t> & FindIfOverlapping(int32_t Key) const
        {
            assert(bFinishedAdding);
            int32_t ContainerIndex = IndexBelongsTo[Key];
            return (ContainerIndex != FBX_INDEX_NONE) ? Arrays[ContainerIndex] : EmptyArray;
        }

    private:
        std::vector<int32_t> IndexBelongsTo;
        std::vector< std::vector<int32_t> > Arrays;
        std::vector< std::set<int32_t> > Sets;
        std::vector<int32_t> EmptyArray;
        bool bFinishedAdding = false;
    };

}