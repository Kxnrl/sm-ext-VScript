## VScript  
Provide vscript variables api to sourcemod, Provide sourcemod api to vscript.

state: W.I.P,  **Very** unstable

### You can check the runtime environment in vscript
```squirrel
try {
    printl("Extension Ver: " + __VScript_Extension);
} catch (e) {
    printl("__VScript_Extension not found.");
    return;
}
```

### Todo
- Add 'SetProp', 'SetData' to VScript
- Add 'Trace*' functions to VScript