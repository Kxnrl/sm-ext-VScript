#include "extension.h"

// windows LTCG optimize to const char*
string_t AllocPooledString(const char* pszValue)
{
    CBaseEntity * pEntity = ((IServerUnknown*)servertools->FirstEntity())->GetBaseEntity();

    static int iNameOffset = -1;
    if (iNameOffset == -1)
    {
        auto* pDataMap = gamehelpers->GetDataMap(pEntity);
        assert(pDataMap);

        sm_datatable_info_t info;
        bool found = gamehelpers->FindDataMapInfo(pDataMap, "m_iName", &info);
        assert(found);
        iNameOffset = info.actual_offset;
    }

    string_t* pProp = (string_t*)((intp)pEntity + iNameOffset);
    string_t backup = *pProp;

    servertools->SetKeyValue(pEntity, "targetname", pszValue);

    string_t newString = *pProp;
    *pProp = backup;

    return newString;
}