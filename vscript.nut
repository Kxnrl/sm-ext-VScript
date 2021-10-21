// You can call this for checking runtime.

if (!("CheckRuntime" in ::getroottable()))
::CheckRuntime <- function() {
    try {
        if (__VScript_Extension)
            return true;
    } catch (e) {
        printl("__VScript_Extension not found.");
    }
    return false;
}