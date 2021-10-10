#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include "ivscript_hack.h"

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
    bool IsAdmin();
    void PrintToChat(const char* message);
    void PrintToHint(const char* message);
};

#define HUD_PRINTNOTIFY   1
#define HUD_PRINTCONSOLE  2
#define HUD_PRINTTALK     3
#define HUD_PRINTCENTER   4

extern sp_nativeinfo_t g_Natives[];

#endif