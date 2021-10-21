#ifndef _INCLUDE_VSCRIPT_UTILS_H_
#define _INCLUDE_VSCRIPT_UTILS_H_

#include "extension.h"

enum PropFieldType
{
	PropField_Unsupported,		/**< The type is unsupported. */
	PropField_Integer,			/**< Valid for SendProp and Data fields */
	PropField_Float,			/**< Valid for SendProp and Data fields */
	PropField_Entity,			/**< Valid for Data fields only (SendProp shows as int) */
	PropField_Vector,			/**< Valid for SendProp and Data fields */
	PropField_String,			/**< Valid for SendProp and Data fields */
	PropField_String_T,			/**< Valid for Data fields.  Read only! */
	PropField_Variant,			/**< Valid for variants/any. (User must know type) */
};

string_t AllocPooledString(const char* pszValue);
void GuessDataPropTypes(typedescription_t* td, cell_t* pSize, cell_t* pType);

#endif
