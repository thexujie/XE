#include "FbxModel.h"

namespace fbx
{
    bool ModelData::DoSectionsNeedExtraBoneInfluences() const
    {
        for (size_t SectionIdx = 0; SectionIdx < Sections.size(); ++SectionIdx)
        {
            if (Sections[SectionIdx].HasExtraBoneInfluences())
            {
                return true;
            }
        }

        return false;
    }
}
