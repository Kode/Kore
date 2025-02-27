#include <kore3/audio/audio.h>

#include <kore3/backend/microsoft.h>

#include <kore3/error.h>
#include <kore3/log.h>
#include <kore3/threads/thread.h>

#include <AudioClient.h>
#include <Windows.h>
#include <initguid.h>
#ifdef KORE_WINDOWSAPP
#include <mfapi.h>
#endif
#include <mmdeviceapi.h>
#include <wrl/implements.h>

#ifdef KORE_WINDOWSAPP
using namespace ::Microsoft::WRL;
using namespace Windows::Media::Devices;
using namespace Windows::Storage::Streams;
#endif

template <class T> void SafeRelease(__deref_inout_opt T **ppT) {
	T *pTTemp = *ppT;
	*ppT = nullptr;
	if (pTTemp) {
		pTTemp->Release();
	}
}

#define SAFE_RELEASE(punk)                                                                                                                                     \
	if ((punk) != NULL) {                                                                                                                                      \
		(punk)->Release();                                                                                                                                     \
		(punk) = NULL;                                                                                                                                         \
	}

// based on the implementation in soloud and Microsoft sample code
namespace {
	kore_thread thread;
	kore_audio_buffer audio_buffer;

	IMMDeviceEnumerator *deviceEnumerator;
	IMMDevice *device;
	IAudioClient *audioClient = NULL;
	IAudioRenderClient *renderClient = NULL;
	HANDLE bufferEndEvent = 0;
	UINT32 bufferFrames;
	WAVEFORMATEX requestedFormat;
	WAVEFORMATEX *closestFormat;
	WAVEFORMATEX *format;
	static uint32_t samples_per_second = 44100;

	bool initDefaultDevice();
	void audioThread(LPVOID);

#ifdef KORE_WINDOWSAPP
	class AudioRenderer : public RuntimeClass<RuntimeClassFlags<ClassicCom>, FtmBase, IActivateAudioInterfaceCompletionHandler> {
	public:
		STDMETHOD(ActivateCompleted)(IActivateAudioInterfaceAsyncOperation *operation) {
			IUnknown *audioInterface = nullptr;
			HRESULT hrActivateResult = S_OK;
			HRESULT hr = operation->GetActivateResult(&hrActivateResult, &audioInterface);
			if (SUCCEEDED(hr) && SUCCEEDED(hrActivateResult)) {
				audioInterface->QueryInterface(IID_PPV_ARGS(&audioClient));
				initDefaultDevice();
				audioThread(nullptr);
			}
			return S_OK;
		}
	};

	ComPtr<AudioRenderer> renderer;
#endif

	bool initDefaultDevice() {
#ifdef KORE_WINDOWSAPP
		HRESULT hr = S_OK;
#else
		if (renderClient != NULL) {
			renderClient->Release();
			renderClient = NULL;
		}

		if (audioClient != NULL) {
			audioClient->Release();
			audioClient = NULL;
		}

		if (bufferEndEvent != 0) {
			CloseHandle(bufferEndEvent);
			bufferEndEvent = 0;
		}

		kore_log(KORE_LOG_LEVEL_INFO, "Initializing a new default audio device.");

		HRESULT hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
		if (hr == S_OK) {
			hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, 0, reinterpret_cast<void **>(&audioClient));
		}
#endif

		if (hr == S_OK) {
			const int sampleRate = 48000;

			format = &requestedFormat;
			memset(&requestedFormat, 0, sizeof(WAVEFORMATEX));
			requestedFormat.nChannels = 2;
			requestedFormat.nSamplesPerSec = sampleRate;
			requestedFormat.wFormatTag = WAVE_FORMAT_PCM;
			requestedFormat.wBitsPerSample = sizeof(short) * 8;
			requestedFormat.nBlockAlign = (requestedFormat.nChannels * requestedFormat.wBitsPerSample) / 8;
			requestedFormat.nAvgBytesPerSec = requestedFormat.nSamplesPerSec * requestedFormat.nBlockAlign;
			requestedFormat.cbSize = 0;

			HRESULT supported = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, format, &closestFormat);
			if (supported == S_FALSE) {
				kore_log(KORE_LOG_LEVEL_WARNING, "Falling back to the system's preferred WASAPI mix format.", supported);
				if (closestFormat != nullptr) {
					format = closestFormat;
				}
				else {
					audioClient->GetMixFormat(&format);
				}
			}
			HRESULT result = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 40 * 1000 * 10, 0, format, 0);
			if (result != S_OK) {
				kore_log(KORE_LOG_LEVEL_WARNING, "Could not initialize WASAPI audio, going silent (error code 0x%x).", result);
				return false;
			}

			uint32_t old_samples_per_second = samples_per_second;
			samples_per_second = format->nSamplesPerSec;
			if (samples_per_second != old_samples_per_second) {
				kore_audio_internal_sample_rate_callback();
			}
			audio_buffer.channel_count = 2;

			bufferFrames = 0;
			kore_microsoft_affirm(audioClient->GetBufferSize(&bufferFrames));
			kore_microsoft_affirm(audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void **>(&renderClient)));

			bufferEndEvent = CreateEvent(0, FALSE, FALSE, 0);
			kore_affirm(bufferEndEvent != 0);

			kore_microsoft_affirm(audioClient->SetEventHandle(bufferEndEvent));

			return true;
		}
		else {
			kore_log(KORE_LOG_LEVEL_WARNING, "Could not initialize WASAPI audio.");
			return false;
		}
	}

	void copyS16Sample(int16_t *left, int16_t *right) {
		float left_value = *(float *)&audio_buffer.channels[0][audio_buffer.read_location];
		float right_value = *(float *)&audio_buffer.channels[1][audio_buffer.read_location];
		audio_buffer.read_location += 1;
		if (audio_buffer.read_location >= audio_buffer.data_size) {
			audio_buffer.read_location = 0;
		}
		*left = (int16_t)(left_value * 32767);
		*right = (int16_t)(right_value * 32767);
	}

	void copyFloatSample(float *left, float *right) {
		float left_value = *(float *)&audio_buffer.channels[0][audio_buffer.read_location];
		float right_value = *(float *)&audio_buffer.channels[1][audio_buffer.read_location];
		audio_buffer.read_location += 1;
		if (audio_buffer.read_location >= audio_buffer.data_size) {
			audio_buffer.read_location = 0;
		}
		*left = left_value;
		*right = right_value;
	}

	void submitEmptyBuffer(unsigned frames) {
		BYTE *buffer = nullptr;
		HRESULT result = renderClient->GetBuffer(frames, &buffer);
		if (FAILED(result)) {
			return;
		}

		memset(buffer, 0, frames * format->nBlockAlign);

		result = renderClient->ReleaseBuffer(frames, 0);
	}

	void submitBuffer(unsigned frames) {
		BYTE *buffer = nullptr;
		HRESULT result = renderClient->GetBuffer(frames, &buffer);
		if (FAILED(result)) {
			if (result == AUDCLNT_E_DEVICE_INVALIDATED) {
				initDefaultDevice();
				submitEmptyBuffer(bufferFrames);
				audioClient->Start();
			}
			return;
		}

		if (kore_audio_internal_callback(&audio_buffer, frames)) {
			if (format->wFormatTag == WAVE_FORMAT_PCM) {
				for (UINT32 i = 0; i < frames; ++i) {
					copyS16Sample((int16_t *)&buffer[i * format->nBlockAlign], (int16_t *)&buffer[i * format->nBlockAlign + 2]);
				}
			}
			else {
				for (UINT32 i = 0; i < frames; ++i) {
					copyFloatSample((float *)&buffer[i * format->nBlockAlign], (float *)&buffer[i * format->nBlockAlign + 4]);
				}
			}
		}
		else {
			memset(buffer, 0, frames * format->nBlockAlign);
		}

		result = renderClient->ReleaseBuffer(frames, 0);
		if (FAILED(result)) {
			if (result == AUDCLNT_E_DEVICE_INVALIDATED) {
				initDefaultDevice();
				submitEmptyBuffer(bufferFrames);
				audioClient->Start();
			}
		}
	}

	void audioThread(LPVOID) {
		submitBuffer(bufferFrames);
		audioClient->Start();
		while (true) {
			WaitForSingleObject(bufferEndEvent, INFINITE);
			UINT32 padding = 0;
			HRESULT result = audioClient->GetCurrentPadding(&padding);
			if (FAILED(result)) {
				if (result == AUDCLNT_E_DEVICE_INVALIDATED) {
					initDefaultDevice();
					submitEmptyBuffer(bufferFrames);
					audioClient->Start();
				}
				continue;
			}
			UINT32 frames = bufferFrames - padding;
			submitBuffer(frames);
		}
	}

} // namespace

#ifndef KORE_WINDOWSAPP
extern "C" void kore_windows_co_initialize(void);
#endif

static bool initialized = false;

void kore_a2_init() {
	if (initialized) {
		return;
	}

	kore_audio_internal_init();
	initialized = true;

	audio_buffer.read_location = 0;
	audio_buffer.write_location = 0;
	audio_buffer.data_size = 128 * 1024;
	audio_buffer.channel_count = 2;
	audio_buffer.channels[0] = (float *)malloc(audio_buffer.data_size * sizeof(float));
	audio_buffer.channels[1] = (float *)malloc(audio_buffer.data_size * sizeof(float));

#ifdef KORE_WINDOWSAPP
	renderer = Make<AudioRenderer>();

	IActivateAudioInterfaceAsyncOperation *asyncOp;
	Platform::String ^ deviceId = MediaDevice::GetDefaultAudioRenderId(Windows::Media::Devices::AudioDeviceRole::Default);
	kore_microsoft_affirm(ActivateAudioInterfaceAsync(deviceId->Data(), __uuidof(IAudioClient2), nullptr, renderer.Get(), &asyncOp));
	SafeRelease(&asyncOp);
#else
	kore_windows_co_initialize();
	kore_microsoft_affirm(
	    CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&deviceEnumerator)));

	if (initDefaultDevice()) {
		kore_thread_init(&thread, audioThread, NULL);
	}
#endif
}

uint32_t kore_a2_samples_per_second(void) {
	return samples_per_second;
}

void kore_a2_update() {}

void kore_a2_shutdown() {
	// Wait for last data in buffer to play before stopping.
	// Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

	//	affirm(pAudioClient->Stop());  // Stop playing.

	//	CoTaskMemFree(pwfx);
	//	SAFE_RELEASE(pEnumerator)
	//	SAFE_RELEASE(pDevice)
	//	SAFE_RELEASE(pAudioClient)
	//	SAFE_RELEASE(pRenderClient)
}
