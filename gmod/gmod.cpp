#include <thread>

#include "memory.hpp"

void entry_point() {
	auto warning = memory::symbol_t<void(__cdecl*)(char const*, ...)>::get_symbol(memory::tier0_module, "Warning");
	warning.ptr("I`m gay and you too\n");

	FreeLibraryAndExitThread(memory::dllinstance, 0);
}

BOOL APIENTRY DllMain(HINSTANCE dll_instance, DWORD reason, LPVOID reversed) {
	memory::dllinstance = dll_instance;
	DisableThreadLibraryCalls(dll_instance);
	if (reason == DLL_PROCESS_ATTACH) {
		std::thread(entry_point).detach();
	}
	return TRUE;
}