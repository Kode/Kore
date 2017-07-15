#include "pch.h"

#include "Hololens.winrt.h"
#include <Kore/Vr/VrInterface.h>

using namespace Kore;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;


void* VrInterface::init(void* hinst, const char* title, const char* windowClassName)
{
	return nullptr;
}

void VrInterface::begin() {
	m_main->begin();
}
	
void VrInterface::beginRender(int eye)
{
	m_main->beginRender(eye);
}

SensorState VrInterface::getSensorState(int eye)
{
	return m_main->getSensorState(eye);
}

void VrInterface::endRender(int eye)
{
	m_main->endRender(eye);
}

void VrInterface::warpSwap()
{
	m_main->warpSwap();
}



void VrInterface::ovrShutdown()
{
	
}

void VrInterface::resetHmdPose()
{
	
}

void VrInterface::updateTrackingOrigin(TrackingOrigin origin)
{
	
}

