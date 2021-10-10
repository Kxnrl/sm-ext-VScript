#include "extension.h"
#include "ivscript_hack.h"

#define DBGFLAG_ASSERT
//#define DEBUG

#define VSCRIPT_GLOBAL_PTR_HACK "__VSCRIPT_HACK"


VScript g_Extension;
SMEXT_LINK(&g_Extension);

void* g_pCSGameRulesFn = nullptr;
IScriptVM* g_pScriptVM = nullptr;
CGlobalVars* g_pGlobals = nullptr;
IGameConfig* g_pGameConf = nullptr;
ScriptClassDesc_t* g_pCBaseEntity = nullptr;
ScriptClassDesc_t* g_pCBasePlayer = nullptr;
cell_t g_RegFuntions;
cell_t g_RegVClasses;

ScriptVariant_t g_ScriptVariant;

#ifdef PLATFORM_WINDOWS
static HSCRIPT(__fastcall* g_pGetScriptInstance)(CBaseEntity* entity) = nullptr;
static bool(__fastcall* g_pValidateScriptScope)(CBaseEntity* entity) = nullptr;
#else
static HSCRIPT(__cdecl* g_pGetScriptInstance)(CBaseEntity* entity) = nullptr;
static bool(__cdecl* g_pValidateScriptScope)(CBaseEntity* entity) = nullptr;
#endif

// ???
// I DON'T GIVE A FUCK
#undef RegisterClass

SH_DECL_HOOK1_void(IScriptVM, RegisterFunction, SH_NOATTRIB, 0, ScriptFunctionBinding_t*);
SH_DECL_HOOK1(IScriptVM, RegisterClass, SH_NOATTRIB, 0, bool, ScriptClassDesc_t*);

// def
#define InjectScriptFunctionToClass(pDesc, func, description) ScriptAddFunctionToClassDesc(pDesc, VScript, func, description)
#define ScriptRegisterCBaseEntityFunction(class, script, func, desc) do\
{\
    Assert(class);\
    ScriptFunctionBinding_t* pBinding = &((class)->m_FunctionBindings[(class)->m_FunctionBindings.AddToTail()]);\
    pBinding->m_desc.m_pszDescription = desc;\
    pBinding->m_desc.m_pszScriptName = script;\
    pBinding->m_desc.m_pszFunction = script;\
    ScriptDeduceFunctionSignature(&pBinding->m_desc, (VScript*)(0), &func);\
    pBinding->m_pfnBinding = ScriptCreateBinding(((VScript*)0), &func);\
    pBinding->m_pFunction = ScriptConvertFuncPtrToVoid(&func);\
    pBinding->m_flags = SF_MEMBER_FUNC;\
} while (0)


#define EntityToHScript(pEntity) pEntity ? g_pGetScriptInstance(pEntity) : nullptr
#define HScriptToCBaseEntity(hScript) hScript ? (CBaseEntity*)g_pScriptVM->GetInstanceValue(hScript, g_pCBaseEntity) : nullptr
#define VSCRIPT_PTR() g_pScriptVM->GetValue("__VSCRIPT_HACK", &g_ScriptVariant) ? HScriptToCBaseEntity(g_ScriptVariant.m_hScript) : nullptr

#define VALIDATE_ENTITY_VOID()  do { if (!pEntity) { g_pScriptVM->RaiseException("Accessed null instance"); return;   } } while (0)
#define VALIDATE_ENTITY_RET(t)  do { if (!pEntity) { g_pScriptVM->RaiseException("Accessed null instance"); return t; } } while (0)

#define VALIDATE_SCRIPT_SCOPE(e) g_pValidateScriptScope(e)
#define GET_ENTITY_SCRIPT_SCOPE(e)  *(CScriptScope*)((char*)pEntity + 860) //g_pGetScriptInstance(e)

void VScript::Hook_OnRegisterFunction(ScriptFunctionBinding_t* pFunction)
{
#ifdef DEBUG
    smutils->LogMessage(myself, "g_pScriptVM->RegisterFunction(%s)", pFunction->m_desc.m_pszFunction);
#endif
}

bool VScript::Hook_OnRegisterClass(ScriptClassDesc_t* pClass)
{
    if (strcmp(pClass->m_pszClassname, "CBaseEntity") == 0)
    {
        g_pCBaseEntity = pClass;
        InjectCBaseEntityFunctions(pClass);
    }

    if (strcmp(pClass->m_pszClassname, "CBasePlayer") == 0)
    {
        g_pCBasePlayer = pClass;
        InjectCBasePlayerFunctions(pClass);
    }

#ifdef DEBUG
    bool ret = META_RESULT_ORIG_RET(bool);

    smutils->LogMessage(myself, "g_pScriptVM->RegisterClass(%s, %s) -> %s", pClass->m_pszClassname, pClass->m_pszScriptName, ret ? "true" : "false");
    FOR_EACH_VEC(pClass->m_FunctionBindings, it)
    {
        smutils->LogMessage(myself, "\t\t\t  m_flags=[%d] m_pszScriptName=[%s] m_pszFunction=[%s], m_pszDescription=[%s], m_Parameters=[%d]",
            pClass->m_FunctionBindings[it].m_flags,
            pClass->m_FunctionBindings[it].m_desc.m_pszScriptName,
            pClass->m_FunctionBindings[it].m_desc.m_pszFunction,
            pClass->m_FunctionBindings[it].m_desc.m_pszDescription,
            pClass->m_FunctionBindings[it].m_desc.m_Parameters.Count());
    }

#endif

    RETURN_META_VALUE(MRES_IGNORED, false);
}

HSCRIPT VScriptGetAdminPlayer()
{
    for (int i = 1; i <= playerhelpers->GetMaxClients(); i++)
    {
        IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(i);
        if (pPlayer && pPlayer->IsInGame() && pPlayer->IsAuthorized() && !pPlayer->IsFakeClient())
        {
            AdminId admin = pPlayer->GetAdminId();
            if (admin != INVALID_ADMIN_ID && adminsys->GetAdminFlag(admin, Admin_Root, Access_Real))
            {
                HSCRIPT entity = EntityToHScript(gamehelpers->ReferenceToEntity(i));
                if (entity)
                    return entity;
            }
        }
    }
    return nullptr;
}

int VScript::GetHammerID()
{
    CBaseEntity* pEntity = VSCRIPT_PTR();
    VALIDATE_ENTITY_RET(-1);

    sm_datatable_info_t info;

    static unsigned _offset = 0;
    if (_offset == 0) {
        if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(gamehelpers->ReferenceToEntity(0)), "m_iHammerID", &info)) {
            return -1;
        }
        _offset = info.actual_offset;
    }

    const int m_iHammerID = *(int*)((char*)pEntity + _offset);

    return m_iHammerID;
}

bool VScript::IsAdmin()
{
    CBaseEntity* pEntity = VSCRIPT_PTR();
    VALIDATE_ENTITY_RET(false);

    const int entity = gamehelpers->EntityToBCompatRef(pEntity);
    IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(entity);
    if (pPlayer && pPlayer->IsInGame() && pPlayer->IsAuthorized() && !pPlayer->IsFakeClient())
    {
        AdminId admin = pPlayer->GetAdminId();
        if (admin != INVALID_ADMIN_ID && adminsys->GetAdminFlag(admin, Admin_Root, Access_Real))
        {
            // load
            return true;
        }
    }

    return false;
}

void VScript::PrintToHint(const char* message)
{
    CBaseEntity* pEntity = VSCRIPT_PTR();
    VALIDATE_ENTITY_VOID();

    cell_t index = gamehelpers->EntityToBCompatRef(pEntity);
    IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(index);
    if (!pPlayer || !pPlayer->IsConnected() || pPlayer->IsFakeClient() || pPlayer->IsSourceTV())
    {
        char error[128];
        smutils->Format(error, 128, "Player %d is invalid.", index);
        g_pScriptVM->RaiseException(error);
        return;
    }
    gamehelpers->HintTextMsg(index, message);
}

void VScript::PrintToChat(const char* message)
{
    CBaseEntity* pEntity = VSCRIPT_PTR();
    VALIDATE_ENTITY_VOID();

    cell_t index = gamehelpers->EntityToBCompatRef(pEntity);
    IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(index);
    if (!pPlayer || !pPlayer->IsConnected() || pPlayer->IsFakeClient() || pPlayer->IsSourceTV())
    {
        char error[128];
        smutils->Format(error, 128, "Player %d is invalid.", index);
        g_pScriptVM->RaiseException(error);
        return;
    }
    gamehelpers->TextMsg(index, HUD_PRINTTALK, message);
}

void VScript::OnCoreMapStart(edict_t* pEdictList, int edictCount, int clientMax)
{
    LoadScriptVM();
    InitRoutines(__FUNCTION__);
}

void VScript::OnCoreMapEnd()
{
    g_pCBaseEntity = nullptr;
    g_pCBasePlayer = nullptr;
    g_pScriptVM = nullptr;
}

bool VScript::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
    if (!gameconfs->LoadGameConfigFile("vscript.games", &g_pGameConf, error, maxlength))
        return false;

    if (!g_pGameConf->GetAddress("CCSGameRules::RegisterScriptFunctions", &g_pCSGameRulesFn) || !g_pCSGameRulesFn)
    {
        smutils->Format(error, maxlength, "Could not get CCSGameRules::RegisterScriptFunctions pattern.");
        return false;
    }

    if (!g_pGameConf->GetMemSig("CBaseEntity::GetScriptInstance", (void**)&g_pGetScriptInstance) || !g_pGetScriptInstance)
    {
        smutils->LogError(myself, "Failed to load CBaseEntity::GetScriptInstance function.");
        return false;
    }

    if (!g_pGameConf->GetMemSig("CBaseEntity::GetScriptInstance", (void**)&g_pGetScriptInstance) || !g_pGetScriptInstance)
    {
        smutils->LogError(myself, "Failed to load CBaseEntity::GetScriptInstance function.");
        return false;
    }

    if (!g_pGameConf->GetMemSig("CBaseEntity::ValidateScriptScope", (void**)&g_pValidateScriptScope) || !g_pValidateScriptScope)
    {
        smutils->LogError(myself, "Failed to load CBaseEntity::ValidateScriptScope function.");
        return false;
    }

    // init
    g_pShareSys->RegisterLibrary(myself, "VScripts");
    g_pShareSys->AddNatives(myself, g_Natives);

    // late
    if (late)
    {
        LoadScriptVM();
        InitRoutines(__FUNCTION__);
    }

    return true;
}

void VScript::SDK_OnUnload()
{
    if (g_RegFuntions)
    {
        SH_REMOVE_HOOK_ID(g_RegFuntions);
        g_RegFuntions = 0;
    }

    if (g_RegVClasses)
    {
        SH_REMOVE_HOOK_ID(g_RegVClasses);
        g_RegVClasses = 0;
    }

    if (g_pScriptVM != nullptr)
    {
        Error("You can not unload this extension, that will crash server...");
    }
}

bool VScript::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    g_pGlobals = ismm->GetCGlobals();

    IScriptManager* sm;
    GET_V_IFACE_ANY(GetEngineFactory, sm, IScriptManager, VSCRIPT_INTERFACE_VERSION);
    IScriptVM* vm = sm->CreateVM();
    Assert(vm);
    g_RegFuntions = SH_ADD_VPHOOK(IScriptVM, RegisterFunction, vm, SH_MEMBER(&g_Extension, &VScript::Hook_OnRegisterFunction), true);
    g_RegVClasses = SH_ADD_VPHOOK(IScriptVM, RegisterClass,    vm, SH_MEMBER(&g_Extension, &VScript::Hook_OnRegisterClass),    false);
    sm->DestroyVM(vm);

    return true;
}

void VScript::LoadScriptVM()
{
    cell_t offset;
    if (!g_pGameConf->GetOffset("g_pScriptVM", &offset) || !offset)
    {
        Error("Could not load g_pScriptVM offset.");
    }

    // DO NOT USE META_IFACEPTR(IScriptVM) !!!
    g_pScriptVM = **(IScriptVM***)((char*)g_pCSGameRulesFn + offset);

    Assert(g_pScriptVM);

#ifdef DEBUG
    smutils->LogMessage(myself, "g_pScriptVM = %x | Language = %s", reinterpret_cast<cell_t>(g_pScriptVM), g_pScriptVM->GetLanguageName());
#endif
}

void VScript::InjectCBaseEntityFunctions(ScriptClassDesc_t* pClass)
{
    ScriptRegisterCBaseEntityFunction(pClass, "GetHammerID", VScript::GetHammerID, "Get entity hammer id.");
}

void VScript::InjectCBasePlayerFunctions(ScriptClassDesc_t* pClass)
{
    InjectScriptFunctionToClass(pClass, PrintToChat, "Print to Chat.");
    InjectScriptFunctionToClass(pClass, PrintToHint, "Print to Hint.");
    InjectScriptFunctionToClass(pClass, IsAdmin, "Check player is admin.");
    //ScriptAddFunctionToClassDesc(pClass, VScript, IsAdmin, "Check player is admin.");
    //ScriptRegisterCBaseEntityFunction(pClass, "IsAdmin", VScript::IsAdmin, "Check player is admin.");
}

void VScript::InitRoutines(const char* func)
{
    smutils->LogMessage(myself, "InitRoutines(%s)", func);

    // inject global functions
    ScriptRegisterFunction(g_pScriptVM, VScriptGetAdminPlayer, "Get Admin Player as Entity.");
}

static cell_t Native_GetScopeValue(IPluginContext* pContext, const cell_t* params)
{
    CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
    if (!pEntity)
    {
        return pContext->ThrowNativeError("Entity %d is invalid", params[1]);
    }

    char* key;
    pContext->LocalToString(params[2], &key);

    void* hScript = GET_ENTITY_SCRIPT_SCOPE(pEntity);

    if (hScript == INVALID_HSCRIPT)
    {
        if (!VALIDATE_SCRIPT_SCOPE(pEntity))
        {
            return pContext->ThrowNativeError("Entity %d has not script scope.", params[1]);
        }
    }

    ScriptVariant_t var;
    if (!g_pScriptVM->GetValue((HSCRIPT)hScript, key, &var))
    {
        return pContext->ThrowNativeError("Failed to get script key value for entity %d.", params[1]);
    }

    _fieldtypes type = (_fieldtypes)params[3];

    if (type != var.m_type)
    {
        return pContext->ThrowNativeError("Value type not match paramType = %d | variantType = %d", type, var.m_type);
    }

    switch(type)
    {
    case FIELD_BOOLEAN:
        return var.m_bool;
    case FIELD_FLOAT:
        return sp_ftoc(var.m_float);
    case FIELD_INTEGER:
        return var.m_int;
    case FIELD_CHARACTER:
        return var.m_char;
    case FIELD_HSCRIPT:
        HSCRIPT result = var.m_hScript;
        pEntity = HScriptToCBaseEntity(result);
        return pEntity ? gamehelpers->EntityToBCompatRef(pEntity) : -1;
    }

    return pContext->ThrowNativeError("Error variantType %d", var.m_type);
}

static cell_t Native_GetScopeValueString(IPluginContext* pContext, const cell_t* params)
{
    CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
    if (!pEntity)
    {
        return pContext->ThrowNativeError("Entity %d is invalid", params[1]);
    }

    char* key;
    pContext->LocalToString(params[2], &key);

    void* hScript = GET_ENTITY_SCRIPT_SCOPE(pEntity);

    if (hScript == INVALID_HSCRIPT)
    {
        if (!VALIDATE_SCRIPT_SCOPE(pEntity))
        {
            return pContext->ThrowNativeError("Entity %d has not script scope.", params[1]);
        }
    }

    ScriptVariant_t var;
    if (!g_pScriptVM->GetValue((HSCRIPT)hScript, key, &var))
    {
        return pContext->ThrowNativeError("Failed to get script key value for entity %d.", params[1]);
    }

    if (var.m_type != FIELD_CSTRING)
    {
        return pContext->ThrowNativeError("Value type not match variantType = %d", var.m_type);
    }

    size_t bytes;
    return pContext->StringToLocalUTF8(params[3], params[4], var.m_pszString, &bytes);
}

static cell_t Native_GetScopeValueVector(IPluginContext* pContext, const cell_t* params)
{
    CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
    if (!pEntity)
    {
        return pContext->ThrowNativeError("Entity %d is invalid", params[1]);
    }

    char* key;
    pContext->LocalToString(params[2], &key);

    void* hScript = GET_ENTITY_SCRIPT_SCOPE(pEntity);

    if (hScript == INVALID_HSCRIPT)
    {
        if (!VALIDATE_SCRIPT_SCOPE(pEntity))
        {
            return pContext->ThrowNativeError("Entity %d has not script scope.", params[1]);
        }
    }

    ScriptVariant_t var;
    if (!g_pScriptVM->GetValue((HSCRIPT)hScript, key, &var))
    {
        return pContext->ThrowNativeError("Failed to get script key value for entity %d.", params[1]);
    }

    if (var.m_type != FIELD_VECTOR)
    {
        return pContext->ThrowNativeError("Value type not match variantType = %d", var.m_type);
    }

    cell_t* pVec;
    pContext->LocalToPhysAddr(params[3], &pVec);

    pVec[0] = sp_ftoc(var.m_pVector->x);
    pVec[1] = sp_ftoc(var.m_pVector->y);
    pVec[2] = sp_ftoc(var.m_pVector->z);

    return 1;
}

static cell_t Native_GetGlobalValue(IPluginContext* pContext, const cell_t* params)
{
    char* key;
    pContext->LocalToString(params[1], &key);

    ScriptVariant_t var;
    if (!g_pScriptVM->GetValue(key, &var))
    {
        return pContext->ThrowNativeError("Failed to get global key value for key %s.", key);
    }

    _fieldtypes type = (_fieldtypes)params[2];

    if (type != var.m_type)
    {
        return pContext->ThrowNativeError("Value type not match paramType = %d | variantType = %d", type, var.m_type);
    }

    switch (type)
    {
    case FIELD_BOOLEAN:
        return var.m_bool;
    case FIELD_FLOAT:
        return sp_ftoc(var.m_float);
    case FIELD_INTEGER:
        return var.m_int;
    case FIELD_CHARACTER:
        return var.m_char;
    case FIELD_HSCRIPT:
        HSCRIPT result = var.m_hScript;
        CBaseEntity* pEntity = HScriptToCBaseEntity(result);
        return pEntity ? gamehelpers->EntityToBCompatRef(pEntity) : -1;
    }

    return pContext->ThrowNativeError("Error variantType %d", var.m_type);
}

static cell_t Native_GetGlobalValueString(IPluginContext* pContext, const cell_t* params)
{
    char* key;
    pContext->LocalToString(params[1], &key);

    ScriptVariant_t var;
    if (!g_pScriptVM->GetValue(key, &var))
    {
        return pContext->ThrowNativeError("Failed to get global key value for key %s.", key);
    }

    if (var.m_type != FIELD_CSTRING)
    {
        return pContext->ThrowNativeError("Value type not match variantType = %d", var.m_type);
    }

    size_t bytes;
    return pContext->StringToLocalUTF8(params[3], params[4], var.m_pszString, &bytes);
}

static cell_t Native_GetGlobalValueVector(IPluginContext* pContext, const cell_t* params)
{
    char* key;
    pContext->LocalToString(params[1], &key);

    ScriptVariant_t var;
    if (!g_pScriptVM->GetValue(key, &var))
    {
        return pContext->ThrowNativeError("Failed to get global key value for key %s.", key);
    }

    if (var.m_type != FIELD_VECTOR)
    {
        return pContext->ThrowNativeError("Value type not match variantType = %d", var.m_type);
    }

    cell_t* pVec;
    pContext->LocalToPhysAddr(params[3], &pVec);

    pVec[0] = sp_ftoc(var.m_pVector->x);
    pVec[1] = sp_ftoc(var.m_pVector->y);
    pVec[2] = sp_ftoc(var.m_pVector->z);

    return 1;
}

sp_nativeinfo_t g_Natives[] =
{
    // why not use -1 for global value?
    {"VScript_GetScopeValue",        Native_GetScopeValue},
    {"VScript_GetScopeValueString",  Native_GetScopeValueString},
    {"VScript_GetScopeValueVector",  Native_GetScopeValueVector},

    {"VScript_GetGlobalValue",        Native_GetGlobalValue},
    {"VScript_GetGlobalValueString",  Native_GetGlobalValueString},
    {"VScript_GetGlobalValueVector",  Native_GetGlobalValueVector},

    // todo
    // add set value
    // set requires g_pScriptVM->ClearValue
    // ...

    {nullptr, nullptr}
};