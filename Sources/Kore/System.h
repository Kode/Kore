#pragma once

#include <Kore/global.h>

#include <Kore/Math/Vector.h>
#include <Kore/Window.h>

namespace Kore {
	enum Orientation { OrientationLandscapeLeft, OrientationLandscapeRight, OrientationPortrait, OrientationPortraitUpsideDown, OrientationUnknown };

	namespace System {
		Window *init(const char *name, int width, int height, WindowOptions *win = nullptr, FramebufferOptions *frame = nullptr);

		const char *name();
		int windowWidth(int id = 0);
		int windowHeight(int id = 0);

		bool handleMessages();
		vec2i mousePos();
		void showKeyboard();
		void hideKeyboard();
		bool showsKeyboard();
		void loadURL(const char *title);
		const char *language();
		void vibrate(int ms);
		const char *systemId();
		const char *savePath();
		const char **videoFormats();
		float safeZone();
		bool automaticSafeZone();
		void setSafeZone(float value);

		typedef u64 ticks;

		double frequency();
		ticks timestamp();
		double time();

		void start();
		bool frame();
		void stop();

		void setKeepScreenOn(bool on);

		void login();
		bool waitingForLogin();
		void unlockAchievement(int id);

		void setCallback(void (*callback)(void *), void *data);
		void setForegroundCallback(void (*callback)(void *), void *data);
		void setResumeCallback(void (*callback)(void *), void *data);
		void setPauseCallback(void (*callback)(void *), void *data);
		void setBackgroundCallback(void (*callback)(void *), void *data);
		void setShutdownCallback(void (*callback)(void *), void *data);
		void setOrientationCallback(void (*callback)(Orientation, void *), void *data);
		void setDropFilesCallback(void (*callback)(wchar_t *, void *), void *data);
		void setCutCallback(char *(*callback)(void *), void *data);
		void setCopyCallback(char *(*callback)(void *), void *data);
		void setPasteCallback(void (*callback)(char *, void *), void *data);
		void setLoginCallback(void (*callback)(void *), void *data);
		void setLogoutCallback(void (*callback)(void *), void *data);

		void _shutdown();
		void _callback();
		void _foregroundCallback();
		void _resumeCallback();
		void _pauseCallback();
		void _backgroundCallback();
		void _shutdownCallback();
		void _orientationCallback(Orientation);
		void _dropFilesCallback(wchar_t *);
		char *_cutCallback();
		char *_copyCallback();
		void _pasteCallback(char *);

		void copyToClipboard(const char *);
	}
}
