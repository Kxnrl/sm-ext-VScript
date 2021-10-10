#if defined _VScript_included
  #endinput
#endif
#define _VScript_included

enum variant_t
{
	FIELD_VOID,
	FIELD_FLOAT,
	FIELD_STRING,
	FIELD_VECTOR,
	FIELD_INTEGER = 5,
	FIELD_BOOLEAN,
	FIELD_CHARACTER = 8,
	FIELD_HSCRIPT = 33 // always use for CBaseEntity
}

native  any VScript_GetScopeValue      (int entity, const char[] key, variant_t field);
native  int VScript_GetScopeValueString(int entity, const char[] key, char[] buffer, int maxlen);
native bool VScript_GetScopeValueVector(int entity, const char[] key, float vVector[3]);

native  any VScript_GetGlobalValue      (const char[] key, variant_t field);
native  int VScript_GetGlobalValueString(const char[] key, char[] buffer, int maxlen);
native bool VScript_GetGlobalValueVector(const char[] key, float vVector[3]);

public Extension __ext_VScript =
{
	name = "VScript",
	file = "VScript.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public void __ext_VScript_SetNTVOptional()
{
    MarkNativeAsOptional("VScript_GetScopeValue");
    MarkNativeAsOptional("VScript_GetScopeValueString");
    MarkNativeAsOptional("VScript_GetScopeValueVector");

	MarkNativeAsOptional("VScript_GetGlobalValue");
    MarkNativeAsOptional("VScript_GetGlobalValueString");
    MarkNativeAsOptional("VScript_GetGlobalValueVector");
}
#endif