# You can modify by your own.

if (WIN32)
	install(TARGETS VScript
		RUNTIME DESTINATION addons/sourcemod/extensions
	)
else()
	install(TARGETS VScript
		LIBRARY DESTINATION addons/sourcemod/extensions
	)
endif()

# SM gamedata
install(DIRECTORY distribute/ DESTINATION addons/sourcemod/gamedata
	FILES_MATCHING PATTERN "vscript.games.txt"
)

# SM include
install(DIRECTORY distribute/ DESTINATION addons/sourcemod/scripting
	FILES_MATCHING PATTERN "vscript.inc"
)

# VScript
install(DIRECTORY distribute/ DESTINATION scripts/vscripts
	FILES_MATCHING PATTERN "vscript.nut"
)

set(CPACK_PACKAGE_VENDOR "${PROJECT_NAME}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CMAKE_SYSTEM_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)