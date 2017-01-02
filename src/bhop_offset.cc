/*

	csgo-bhop-linux -- example CreateMove hook for 64-bit CS:GO on Linux
	Read more at https://aixxe.net/2016/09/createmove-linux-csgo/

	Copyright (C) 2017, aixxe. (www.aixxe.net)
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with csgo-bhop-linux. If not, see <http://www.gnu.org/licenses/>.

*/

#include <memory>

#include "sdk.h"
#include "vmthook/vmthook.h"

/* game interface pointers */
IBaseClientDLL* clientdll = nullptr;
IVEngineClient* engine = nullptr;
IClientEntityList* entitylist = nullptr;

/* virtual table hooks */
std::unique_ptr<VMTHook> clientmode_hook;

/* function prototypes */
typedef void* (*GetClientModeFn) ();

/* replacement CreateMove function */
bool hkCreateMove(void* thisptr, float flInputSampleTime, CUserCmd* cmd) {
	/* command number is 0 for extra mouse sampling */
	if (cmd && cmd->command_number) {
		C_BaseEntity* localplayer = reinterpret_cast<C_BaseEntity*>(entitylist->GetClientEntity(engine->GetLocalPlayer()));

		/* only jump when we're on the ground. */
		if (localplayer && cmd->buttons & IN_JUMP && !(localplayer->GetFlags() & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;
	}

	return clientmode_hook->GetOriginalFunction<CreateMoveFn>(25)(thisptr, flInputSampleTime, cmd);
}

/* called when the library is loading */
int __attribute__((constructor)) bhop_init() {
	/* obtain pointers to game interface classes */
	clientdll = GetInterface<IBaseClientDLL>("./csgo/bin/linux64/client_client.so", CLIENT_DLL_INTERFACE_VERSION);
	engine = GetInterface<IVEngineClient>("./bin/linux64/engine_client.so", VENGINE_CLIENT_INTERFACE_VERSION);
	entitylist = GetInterface<IClientEntityList>("./csgo/bin/linux64/client_client.so", VCLIENTENTITYLIST_INTERFACE_VERSION);
	
	/* get the HudProcessInput function address from CHLClient */
	const uintptr_t hudprocessinput = reinterpret_cast<uintptr_t>(GetVirtualTable(clientdll)[10]);

	/* get the GetClientMode function address from 11 bytes in */
	GetClientModeFn GetClientMode = reinterpret_cast<GetClientModeFn>(GetAbsoluteAddress(hudprocessinput + 11, 1, 5));

	/* hook IClientMode::CreateMove */
	clientmode_hook = std::make_unique<VMTHook>(GetClientMode());
	clientmode_hook->HookFunction(reinterpret_cast<void*>(hkCreateMove), 25);

	return 0;
}