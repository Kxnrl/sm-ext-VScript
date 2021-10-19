## VScript  
Provide vscript variables api for sourcemod, Provide sourcemod api for vscript.

state: W.I.P

### You can check the runtime environment in vscript
```squirrel
try {
    printl("Extension Ver: " + __VScript_Extension);
} catch (e) {
    printl("__VScript_Extension not found.");
    return;
}
```