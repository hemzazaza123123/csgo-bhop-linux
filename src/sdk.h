#include <cstdint>
#include <dlfcn.h>

/* interface versions */
#define VENGINE_CLIENT_INTERFACE_VERSION "VEngineClient014"
#define VCLIENTENTITYLIST_INTERFACE_VERSION	"VClientEntityList003"

/* network variable offsets */
#define m_fFlags 0x138

/* player-specific flags */
#define	FL_ONGROUND (1 << 0)

/* command buttons */
#define IN_JUMP (1 << 1)

/* game structures */
struct CUserCmd {
	virtual ~CUserCmd() {};
	int command_number;
	int tick_count;
	float viewangles[3];
	float aimdirection[3];
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	unsigned char impulse;
	int weaponselect;
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
	float headangles[3];
	float headoffset[3];
};

/* function prototypes */
typedef void* (*CreateInterfaceFn) (const char*, int*);
typedef bool (*CreateMoveFn) (void*, float, CUserCmd*);

/* helper functions */
template <typename interface> interface* GetInterface(const char* filename, const char* version) {
	void* library = dlopen(filename, RTLD_NOLOAD | RTLD_NOW);

	if (!library)
		return nullptr;

	void* createinterface_sym = dlsym(library, "CreateInterface");

	if (!createinterface_sym)
		return nullptr;

	CreateInterfaceFn factory = reinterpret_cast<CreateInterfaceFn>(createinterface_sym);

	return reinterpret_cast<interface*>(factory(version, nullptr));
}

void** GetVirtualTable(void* baseclass) {
	return *reinterpret_cast<void***>(baseclass);
};

template <typename Fn> inline Fn GetVirtualFunction(void* baseclass, size_t index) {
	return reinterpret_cast<Fn>(GetVirtualTable(baseclass)[index]);
}

uintptr_t GetAbsoluteAddress(uintptr_t instruction_ptr, int offset, int size) {
	return instruction_ptr + *reinterpret_cast<uint32_t*>(instruction_ptr + offset) + size;
};

/* generic game classes */
class C_BaseEntity {
	public:
		int GetFlags() {
			return *(int*)((uintptr_t)this + m_fFlags);
		}
};

/* game interface classes */
class IVEngineClient {
	public:
		int GetLocalPlayer() {
			return GetVirtualFunction<int(*)(void*)>(this, 12)(this);
		}
};

class IClientEntityList {
	public:
		void* GetClientEntity(int index) {
			return GetVirtualFunction<void*(*)(void*, int)>(this, 3)(this, index);
		}
};

class IClientMode {};