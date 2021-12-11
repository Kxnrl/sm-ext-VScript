#include "utils.h"

#include "extension.h"

// windows LTCG optimize to const char*
string_t AllocPooledString(const char* pszValue)
{
    CBaseEntity* pEntity = ((IServerUnknown*)servertools->FirstEntity())->GetBaseEntity();

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

void GuessDataPropTypes(typedescription_t* td, cell_t* pSize, cell_t* pType)
{
    switch (td->fieldType)
    {
    case FIELD_TICK:
    case FIELD_MODELINDEX:
    case FIELD_MATERIALINDEX:
    case FIELD_INTEGER:
    case FIELD_COLOR32:
    {
        *pType = PropField_Integer;
        *pSize = 32;
        return;
    }
    case FIELD_VECTOR:
    case FIELD_POSITION_VECTOR:
    {
        *pType = PropField_Vector;
        *pSize = 96;
        return;
    }
    case FIELD_SHORT:
    {
        *pType = PropField_Integer;
        *pSize = 16;
        return;
    }
    case FIELD_BOOLEAN:
    {
        *pType = PropField_Integer;
        *pSize = 1;
        return;
    }
    case FIELD_CHARACTER:
    {
        if (td->fieldSize == 1)
        {
            *pType = PropField_Integer;
            *pSize = 8;
        }
        else
        {
            *pType = PropField_String;
            *pSize = 8 * td->fieldSize;
        }
        return;
    }
    case FIELD_MODELNAME:
    case FIELD_SOUNDNAME:
    case FIELD_STRING:
    {
        *pSize = sizeof(string_t);
        *pType = PropField_String_T;
        return;
    }
    case FIELD_FLOAT:
    case FIELD_TIME:
    {
        *pType = PropField_Float;
        *pSize = 32;
        return;
    }
    case FIELD_EHANDLE:
    {
        *pType = PropField_Entity;
        *pSize = 32;
        return;
    }
    case FIELD_CUSTOM:
    {
        if ((td->flags & FTYPEDESC_OUTPUT) == FTYPEDESC_OUTPUT)
        {
            *pType = PropField_Variant;
            *pSize = 0;
            return;
        }
    }
    }

    *pType = PropField_Unsupported;
    *pSize = 0;
}