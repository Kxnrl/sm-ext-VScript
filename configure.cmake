set(SDK_USE_VENDOR ON CACHE INTERNAL "Determine the hl2sdk uses local folder or environment variable.")

# Only tested in windows, lin is WIP.
set(ENABLE_PDB ON CACHE INTERNAL "Enable debugging symbols (aka pdb).")

set(ENABLE_OPTIMIZE ON CACHE INTERNAL "Enable optimization.")

set(SM_USE_VENDOR ON CACHE INTERNAL "Determine the SourceMod path uses local folder or environment variable.")
set(MMS_USE_VENDOR ON CACHE INTERNAL "Determine the MetaMod:Source path uses local folder or environment variable.")

set(SOURCE2 OFF CACHE INTERNAL "Do we need to build source 2?")
if (NOT SOURCE2)
    set(HL2SDK_ENGINE_NAME "" CACHE INTERNAL "Build against specified SDKs.")
endif()
set(TARGET_ARCH "" CACHE INTERNAL "Override the target architecture. (x86|x64)")
