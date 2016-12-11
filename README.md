![csgo-bhop-linux](https://cdn.aixxe.net/projects/csgo-bhop-linux/logo-transparent.png)

# csgo-bhop-linux

Example auto-bunnyhop via `IClientMode::CreateMove` virtual method hooking.

Methodology is detailed in the [blog post](https://aixxe.net/2016/09/createmove-linux-csgo/) here.

## Usage

Two methods are available. You only need to compile one, the end result is the same.

### Pattern scanning

Finds the `IClientMode` pointer inside the [`CCSModeManager::Init`](https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/client/hl2/hl2_clientmode.cpp#L46-L50) function.

```
g++ -std=c++14 -m64 -fPIC -o bhop_patternscan.so src/bhop_patternscan.cc -shared
```

### Offseting

Finds the `IClientMode` function from 11 bytes in the [`CHLClient::HudProcessInput`](https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/cdll_client_int.cpp#L1252-L1255) function.

```
g++ -std=c++14 -m64 -fPIC -o bhop_offset.so src/bhop_offset.cc -shared
```

Load into game process with preferred method, see my [blog post](https://aixxe.net/2016/09/shared-library-injection) for some options.