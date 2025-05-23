#ifdef KORE_LIVEPP
#include <kore3/system.h>

#include <Windows.h>

#include "LivePP/API/LPP_API_x64_CPP.h"

static lpp::LppDefaultAgent lppAgent;

void kore_LivePP_start(void) {
	// create a default agent, loading the Live++ agent from the given path, e.g. "ThirdParty/LivePP"
	lppAgent = lpp::LppCreateDefaultAgent(KORE_LIVEPP_PATH);

	// enable Live++ for all loaded modules
	lppAgent.EnableModule(lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES);
}

void kore_LivePP_stop(void) {
	// destroy the Live++ agent
	lpp::LppDestroyDefaultAgent(&lppAgent);
}
#else
void kore_start_LivePP(void) {}
void kore_stop_LivePP(void) {}
#endif
