@echo off
IF NOT EXIST wasm mkdir wasm

emcc --bind -o wasm/game.js ../game/quickmake_game.cpp ./bindings/bindings_wrapper.cpp -O2 -s WASM=1 -s MODULARIZE=1 -s TOTAL_MEMORY=209715200 -s EXPORTED_FUNCTIONS="['_updateGame','_loadNextAssetFile','_getGameSoundSamples','_loadFile']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'writeAsciiToMemory', 'lengthBytesUTF8']" --post-js ./bindings/bindings.js
