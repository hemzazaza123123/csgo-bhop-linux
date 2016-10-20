/*

	csgo-bhop-linux -- example CreateMove hook for 64-bit CS:GO on Linux
	Read more at https://aixxe.net/2016/09/createmove-linux-csgo/

	Copyright (C) 2016, aixxe. (www.aixxe.net)
	
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

#include <link.h>
#include <memory>

#include "sdk.h"
#include "findpattern.h"
#include "vmthook/vmthook.h"

/* signature to CCSModeManager::Init */
#define CCSMODEMANAGER_INIT_SIGNATURE "\x48\x8D\x05\x00\x00\x00\x00\x48\x89\xE5\x48\x89\x05\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x5D\x48"
#define CCSMODEMANAGER_INIT_MASK "xxx????xxxxxx????x????xx"

/* game interface pointers */
IVEngineClient* engine = nullptr;
IClientEntityList* entitylist = nullptr;
IClientMode* clientmode = nullptr;

/* virtual table hooks */
std::unique_ptr<VMTHook> clientmode_hook;

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
	engine = GetInterface<IVEngineClient>("./bin/linux64/engine_client.so", VENGINE_CLIENT_INTERFACE_VERSION);
	entitylist = GetInterface<IClientEntityList>("./csgo/bin/linux64/client_client.so", VCLIENTENTITYLIST_INTERFACE_VERSION);
	
	/* get client_client.so base address */
	uintptr_t client_client = 0;

	dl_iterate_phdr([] (struct dl_phdr_info* info, size_t size, void* data) {
		if (strcasestr(info->dlpi_name, "client_client.so")) {
			*reinterpret_cast<uintptr_t*>(data) = info->dlpi_addr + info->dlpi_phdr[0].p_vaddr;
			return 1;
		}

		return 0;
	}, &client_client);

	/* scan client_client.so for our signature - real scan length is left as an exercise for the reader */
	uintptr_t init_address = FindPattern(client_client, 0xFFFFFFFFF, (unsigned char*)CCSMODEMANAGER_INIT_SIGNATURE, CCSMODEMANAGER_INIT_MASK);

	if (!init_address)
		return 1;

	/* get the global IClientMode pointer */
	clientmode = reinterpret_cast<IClientMode*>(GetAbsoluteAddress(init_address, 3, 7));

	/* hook IClientMode::CreateMove */
	clientmode_hook = std::make_unique<VMTHook>(clientmode);
	clientmode_hook->HookFunction(reinterpret_cast<void*>(hkCreateMove), 25);

	return 0;
}