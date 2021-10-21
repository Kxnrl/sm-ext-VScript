#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include "ivscript_hack.h"
#include "itoolentity.h"
#include "smsdk_ext.h"

class VScript : public SDKExtension
{
public:
    // SDKExtension
    virtual bool SDK_OnLoad(char* error, size_t maxlength, bool late);
    virtual void SDK_OnUnload();
    virtual bool SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlength, bool late);

    virtual void OnCoreMapStart(edict_t* pEdictList, int edictCount, int clientMax);
    virtual void OnCoreMapEnd();

private:
    // extension
    void LoadScriptVM();

public:
    // sourcehook
    void Hook_OnRegisterFunction(ScriptFunctionBinding_t* pFunction);
    bool Hook_OnRegisterClass(ScriptClassDesc_t* pClass);
private:
    // sourcehook
    void InjectCBaseEntityFunctions(ScriptClassDesc_t* pClass);
    void InjectCBasePlayerFunctions(ScriptClassDesc_t* pClass);
    void InitRoutines(const char* func);

public:
    // functions
    int GetHammerID();
    int GetUserID();
    const char* GetSteamID();
    bool IsAdmin();
    void PrintToChat(const char* message);
    void PrintToHint(const char* message);

    // prop/data
    void SetEdictStateChanged();

    // get
    int GetDataInt(const char* prop);
    float GetDataFloat(const char* prop);
    HSCRIPT GetDataEntity(const char* prop);
    const Vector& GetDataVector(const char* prop);
    const char* GetDataString(const char* prop);

    // set
    void SetDataInt(const char* prop, int value);
    void SetDataFloat(const char* prop, float value);
    void SetDataEntity(const char* prop, HSCRIPT hEntity);
    void SetDataVector(const char* prop, const Vector& vector);
    void SetDataString(const char* prop, const char* string);

public:
    bool m_bLoaded;
};

#define HUD_PRINTNOTIFY   1
#define HUD_PRINTCONSOLE  2
#define HUD_PRINTTALK     3
#define HUD_PRINTCENTER   4

extern sp_nativeinfo_t g_Natives[];
extern IServerTools* servertools;
extern IScriptVM* g_pScriptVM;

inline void VScriptRaiseException(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char error[128];
    smutils->FormatArgs(error, sizeof(error), fmt, ap);
    va_end(ap);
    g_pScriptVM->RaiseException(error);
}

#endif