![csgo-bhop-linux](https://cdn.aixxe.net/projects/csgo-bhop-linux/logo-transparent.png)

# csgo-bhop-linux

Example auto-bunnyhop via `IClientMode::CreateMove` virtual method hooking. Uses signature scanning to obtain the `IClientMode` pointer. Methodology is detailed in the [blog post](https://aixxe.net/2016/09/createmove-linux-csgo/) here.

## Usage
```
g++ -std=c++14 -m64 -fPIC -fno-use-cxa-atexit -c -o src/bhop.o src/bhop.cc
g++ src/bhop.o -o bhop.so -m64 -shared
```
Load into game process with preferred method, see my [blog post](https://aixxe.net/2016/09/shared-library-injection) for some options.