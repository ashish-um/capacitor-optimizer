# Trying to achieve:-
<img src="https://github.com/user-attachments/assets/4aa63475-bee4-4535-a4ea-0d8af4dcd757" alt="drawing" width="700"/>

uses webassembly to run the cpp code in browser  
deployed working prototype:-
https://ashish-um.github.io/capacitor-optimizer/


### Export command:
```
emcc main.cpp -o capacitor.js -s WASM=1 -s EXPORTED_FUNCTIONS="['_malloc', '_free', '_setup', '_run', '_get_result', '_get_config']" -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'HEAPF64', 'HEAP32', 'UTF8ToString']"
```
