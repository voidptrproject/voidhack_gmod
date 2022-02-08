#include <Windows.h>

#include <thread>

void entry_point() {

}

BOOL APIENTRY DllMain(HINSTANCE dll_instance, DWORD reason, LPVOID reversed) {
	DisableThreadLibraryCalls(dll_instance);
	if (reason == DLL_PROCESS_ATTACH) {
		std::thread(entry_point).detach();
	}
	return TRUE;
}