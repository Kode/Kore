// #include <kore3/graphics4/graphics.h>
#include <kore3/input/gamepad.h>
#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
#include <kore3/system.h>
#include <kore3/threads/thread.h>
#include <kore3/video.h>
#include <kore3/window.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#ifndef XINPUT
#ifdef KORE_WINDOWS
#define XINPUT 1
#endif

#ifdef KORE_WINDOWSAPP
#define XINPUT !(WINAPI_PARTITION_PHONE_APP)
#endif
#endif
#if XINPUT
#include <Xinput.h>
#endif

#include <d3d11_1.h>
#include <d3d11_4.h>
#include <dxgi1_5.h>
#include <wrl.h>

#ifdef KORE_WINDOWSAPP
using namespace ::Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
#ifdef KORE_HOLOLENS
using namespace Windows::Graphics::Holographic;
using namespace Windows::Graphics::DirectX::Direct3D11;
#endif
#endif

extern "C" IUnknown *kore_winapp_internal_get_window(void) {
	return reinterpret_cast<IUnknown *>(CoreWindow::GetForCurrentThread());
}

extern "C" void kore_internal_uwp_installed_location_path(char *path) {
	Platform::String ^ locationString = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
	WideCharToMultiByte(CP_UTF8, 0, locationString->Begin(), -1, path, 1000, nullptr, nullptr);
}

ref class Win8Application sealed : public Windows::ApplicationModel::Core::IFrameworkView {
public:
	Win8Application();
	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView ^ applicationView);
	virtual void SetWindow(Windows::UI::Core::CoreWindow ^ window);
	virtual void Load(Platform::String ^ entryPoint);
	virtual void Run();
	virtual void Uninitialize();

protected:
	void OnWindowSizeChanged(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::WindowSizeChangedEventArgs ^ args);
	// void OnLogicalDpiChanged(Platform::Object^ sender);
	void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView ^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs ^ args);
	void OnSuspending(Platform::Object ^ sender, Windows::ApplicationModel::SuspendingEventArgs ^ args);
	void OnResuming(Platform::Object ^ sender, Platform::Object ^ args);
	void OnWindowClosed(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::CoreWindowEventArgs ^ args);
	void OnPointerPressed(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
	void OnPointerReleased(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);
	void OnPointerMoved(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args);

private:
	// CubeRenderer^ m_renderer;
	// Windows::ApplicationModel::Core::CoreApplicationView^ view;
	bool closed;
	void OnKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);
	void OnKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args);
};

ref class Win8ApplicationSource : Windows::ApplicationModel::Core::IFrameworkViewSource {
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView ^ CreateView();
};

namespace {
	int mouseX, mouseY;
	float axes[12 * 6];
	float buttons[12 * 16];
}

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

const char *kore_gamepad_vendor(int gamepad) {
	return "Microsoft";
}

const char *kore_gamepad_product_name(int gamepad) {
	return "Xbox 360 Controller";
}

bool kore_internal_handle_messages(void) {
	CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

#if XINPUT
	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		dwResult = XInputGetState(i, &state);

		if (dwResult == ERROR_SUCCESS) {
			float newaxes[6];
			newaxes[0] = state.Gamepad.sThumbLX / 32768.0f;
			newaxes[1] = state.Gamepad.sThumbLY / 32768.0f;
			newaxes[2] = state.Gamepad.sThumbRX / 32768.0f;
			newaxes[3] = state.Gamepad.sThumbRY / 32768.0f;
			newaxes[4] = state.Gamepad.bLeftTrigger / 255.0f;
			newaxes[5] = state.Gamepad.bRightTrigger / 255.0f;
			for (int i2 = 0; i2 < 6; ++i2) {
				if (axes[i * 6 + i2] != newaxes[i2]) {
					kore_internal_gamepad_trigger_axis(i, i2, newaxes[i2]);
					axes[i * 6 + i2] = newaxes[i2];
				}
			}
			float newbuttons[16];
			newbuttons[0] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 1.0f : 0.0f;
			newbuttons[1] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 1.0f : 0.0f;
			newbuttons[2] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 1.0f : 0.0f;
			newbuttons[3] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 1.0f : 0.0f;
			newbuttons[4] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 1.0f : 0.0f;
			newbuttons[5] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 1.0f : 0.0f;
			newbuttons[6] = state.Gamepad.bLeftTrigger / 255.0f;
			newbuttons[7] = state.Gamepad.bRightTrigger / 255.0f;
			newbuttons[8] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 1.0f : 0.0f;
			newbuttons[9] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 1.0f : 0.0f;
			newbuttons[10] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 1.0f : 0.0f;
			newbuttons[11] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 1.0f : 0.0f;
			newbuttons[12] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 1.0f : 0.0f;
			newbuttons[13] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 1.0f : 0.0f;
			newbuttons[14] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 1.0f : 0.0f;
			newbuttons[15] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1.0f : 0.0f;
			for (int i2 = 0; i2 < 16; ++i2) {
				if (buttons[i * 16 + i2] != newbuttons[i2]) {
					kore_internal_gamepad_trigger_button(i, i2, newbuttons[i2]);
					buttons[i * 16 + i2] = newbuttons[i2];
				}
			}
		}
	}
#endif

	return true;
}

// Kore::vec2i Kore::System::mousePos() {
//	return vec2i(mouseX, mouseY);
//}

#undef CreateWindow

int kore_init(const char *name, int width, int height, struct kore_window_parameters *win, struct kore_framebuffer_parameters *frame) {
	kore_window_parameters defaultWin;
	if (win == NULL) {
		kore_window_options_set_defaults(&defaultWin);
		win = &defaultWin;
	}
	win->width = width;
	win->height = height;

	kore_framebuffer_parameters defaultFrame;
	if (frame == NULL) {
		kore_framebuffer_options_set_defaults(&defaultFrame);
		frame = &defaultFrame;
	}

	// kore_g4_internal_init();
	// kore_g4_internal_init_window(0, frame->depth_bits, frame->stencil_bits, true);

	return 0;
}

void kore_set_keep_screen_on(bool on) {}

namespace {
	bool keyboardshown = false;
	char language[3] = {0};
}

void kore_keyboard_show() {
	keyboardshown = true;
}

void kore_keyboard_hide() {
	keyboardshown = false;
}

bool kore_keyboard_active() {
	return keyboardshown;
}

void kore_load_url(const char *url) {}

void kore_vibrate(int ms) {}

const char *kore_language() {
	wchar_t wlanguage[3] = {0};

	if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO639LANGNAME, wlanguage, 3)) {
		WideCharToMultiByte(CP_UTF8, 0, wlanguage, -1, language, 3, nullptr, nullptr);
		return language;
	}
	return "en";
}

Win8Application::Win8Application() : closed(false) {}

void Win8Application::Initialize(CoreApplicationView ^ applicationView) {
	applicationView->Activated += ref new TypedEventHandler<CoreApplicationView ^, IActivatedEventArgs ^>(this, &Win8Application::OnActivated);
	CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs ^>(this, &Win8Application::OnSuspending);
	CoreApplication::Resuming += ref new EventHandler<Platform::Object ^>(this, &Win8Application::OnResuming);
	// m_renderer = ref new CubeRenderer();
}

void Win8Application::SetWindow(CoreWindow ^ window) {
	window->SizeChanged += ref new TypedEventHandler<CoreWindow ^, WindowSizeChangedEventArgs ^>(this, &Win8Application::OnWindowSizeChanged);
	window->Closed += ref new TypedEventHandler<CoreWindow ^, CoreWindowEventArgs ^>(this, &Win8Application::OnWindowClosed);
	window->PointerPressed += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(this, &Win8Application::OnPointerPressed);
	window->PointerReleased += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(this, &Win8Application::OnPointerReleased);
	window->PointerMoved += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(this, &Win8Application::OnPointerMoved);
	window->KeyDown +=
	    ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(this, &Win8Application::OnKeyDown);
	window->KeyUp +=
	    ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(this, &Win8Application::OnKeyUp);
	// m_renderer->Initialize(CoreWindow::GetForCurrentThread());

#ifdef KORE_HOLOLENS
	// Create holographics space - needs to be created before window is activated
	holographicFrameController = std::make_unique<HolographicFrameController>(window);

	// create video frame processor
	VideoFrameProcessor::createAsync().then([this](std::shared_ptr<VideoFrameProcessor> videoProcessor) { videoFrameProcessor = std::move(videoProcessor); });
#endif
}

void Win8Application::Load(Platform::String ^ entryPoint) {}

void Win8Application::Run() {
	// BasicTimer^ timer = ref new BasicTimer();
	kickstart(0, nullptr);
	// while (!closed) {
	// timer->Update();

	// m_renderer->Update(timer->Total, timer->Delta);
	// m_renderer->Render();
	// m_renderer->Present(); // This call is synchronized to the display frame rate.
	//}
}

void Win8Application::Uninitialize() {}

int kore_uwp_window_width;
int kore_uwp_window_height;

void Win8Application::OnWindowSizeChanged(CoreWindow ^ sender, WindowSizeChangedEventArgs ^ args) {
	kore_uwp_window_width = (int)args->Size.Width;
	kore_uwp_window_height = (int)args->Size.Height;
	// kore_internal_resize(0, (int)args->Size.Width, (int)args->Size.Height); // TODO
}

void Win8Application::OnWindowClosed(CoreWindow ^ sender, CoreWindowEventArgs ^ args) {
	closed = true;
}

void Win8Application::OnActivated(CoreApplicationView ^ applicationView, IActivatedEventArgs ^ args) {
	CoreWindow::GetForCurrentThread()->Activate();
}

void Win8Application::OnSuspending(Platform::Object ^ sender, SuspendingEventArgs ^ args) {
	// SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
	// deferral->Complete();
}

void Win8Application::OnResuming(Platform::Object ^ sender, Platform::Object ^ args) {}

void Win8Application::OnPointerPressed(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args) {
	mouseX = static_cast<int>(args->CurrentPoint->Position.X);
	mouseY = static_cast<int>(args->CurrentPoint->Position.Y);
	kore_internal_mouse_trigger_press(0, 0, mouseX, mouseY);
}

void Win8Application::OnPointerReleased(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args) {
	mouseX = static_cast<int>(args->CurrentPoint->Position.X);
	mouseY = static_cast<int>(args->CurrentPoint->Position.Y);
	kore_internal_mouse_trigger_release(0, 0, mouseX, mouseY);
}

void Win8Application::OnPointerMoved(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::PointerEventArgs ^ args) {
	mouseX = static_cast<int>(args->CurrentPoint->Position.X);
	mouseY = static_cast<int>(args->CurrentPoint->Position.Y);
	kore_internal_mouse_trigger_move(0, mouseX, mouseY);
}

void Win8Application::OnKeyDown(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args) {
	switch (args->VirtualKey) {
	case Windows::System::VirtualKey::Left:
		kore_internal_keyboard_trigger_key_down(KORE_KEY_LEFT);
		break;
	case Windows::System::VirtualKey::Right:
		kore_internal_keyboard_trigger_key_down(KORE_KEY_RIGHT);
		break;
	case Windows::System::VirtualKey::Up:
		kore_internal_keyboard_trigger_key_down(KORE_KEY_UP);
		break;
	case Windows::System::VirtualKey::Down:
		kore_internal_keyboard_trigger_key_down(KORE_KEY_DOWN);
		break;
	default:
		if (args->VirtualKey >= Windows::System::VirtualKey::A && args->VirtualKey <= Windows::System::VirtualKey::Z) {
			kore_internal_keyboard_trigger_key_down((int)args->VirtualKey);
		}
		else if (args->VirtualKey >= Windows::System::VirtualKey::Number0 && args->VirtualKey <= Windows::System::VirtualKey::Number9) {
			kore_internal_keyboard_trigger_key_down((int)args->VirtualKey);
		}
		break;
	}
}

void Win8Application::OnKeyUp(Windows::UI::Core::CoreWindow ^ sender, Windows::UI::Core::KeyEventArgs ^ args) {
	switch (args->VirtualKey) {
	case Windows::System::VirtualKey::Left:
		kore_internal_keyboard_trigger_key_up(KORE_KEY_LEFT);
		break;
	case Windows::System::VirtualKey::Right:
		kore_internal_keyboard_trigger_key_up(KORE_KEY_RIGHT);
		break;
	case Windows::System::VirtualKey::Up:
		kore_internal_keyboard_trigger_key_up(KORE_KEY_UP);
		break;
	case Windows::System::VirtualKey::Down:
		kore_internal_keyboard_trigger_key_up(KORE_KEY_DOWN);
		break;
	default:
		if (args->VirtualKey >= Windows::System::VirtualKey::A && args->VirtualKey <= Windows::System::VirtualKey::Z) {
			kore_internal_keyboard_trigger_key_up((int)args->VirtualKey);
		}
		else if (args->VirtualKey >= Windows::System::VirtualKey::Number0 && args->VirtualKey <= Windows::System::VirtualKey::Number9) {
			kore_internal_keyboard_trigger_key_up((int)args->VirtualKey);
		}
		break;
	}
}

IFrameworkView ^ Win8ApplicationSource::CreateView() {
	return ref new Win8Application;
}

const char *kore_internal_save_path() {
	return "\\";
}

const char *kore_system_id() {
	return "WindowsApp";
}

namespace {
	const char *videoFormats[] = {"ogv", nullptr};
}

const char **kore_video_formats() {
	return ::videoFormats;
}

int kore_window_width(int window_index) {
	return kore_uwp_window_width;
}

int kore_window_height(int window_index) {
	return kore_uwp_window_height;
}

double kore_frequency() {
	kore_ticks rate;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&rate));
	return (double)rate;
}

void kore_internal_shutdown() {}

static kore_ticks start_stamp;

kore_ticks kore_timestamp() {
	kore_ticks stamp;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&stamp));
	return stamp - start_stamp;
}

void kore_login() {}

void kore_unlock_achievement(int id) {}

void kore_gamepad_set_count(int count) {}

bool kore_gamepad_connected(int num) {
	return true;
}

void kore_gamepad_rumble(int gamepad, float left, float right){

}

    [Platform::MTAThread] int main(Platform::Array<Platform::String ^> ^) {
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER *>(&start_stamp));
	CoreApplication::Run(ref new Win8ApplicationSource);
	return 0;
}

int kore_cpu_cores(void) {
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION info[1024];
	DWORD returnLength = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) * 1024;
	BOOL success = GetLogicalProcessorInformation(&info[0], &returnLength);

	int proper_cpu_count = 0;

	if (success) {
		DWORD byteOffset = 0;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = &info[0];
		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) {
			if (ptr->Relationship == RelationProcessorCore) {
				++proper_cpu_count;
			}

			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			++ptr;
		}
	}
	else {
		proper_cpu_count = 1;
	}

	return proper_cpu_count;
}

int kore_hardware_threads(void) {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}
