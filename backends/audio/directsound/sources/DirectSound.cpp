#include <kore3/audio/audio.h>

#include <kinc/system.h>

#include <kinc/backend/SystemMicrosoft.h>
#include <kinc/backend/Windows.h>

#include <dsound.h>

#include <assert.h>

namespace {
	IDirectSound8 *dsound = nullptr;
	IDirectSoundBuffer *dbuffer = nullptr;
	const DWORD dsize = 50 * 1024;
	const int samplesPerSecond = 44100;
	const int bitsPerSample = 16;

	DWORD lastPlayPosition = 0;
	bool secondHalfFilled = false;

	const int gap = 10 * 1024;
	DWORD writePos = gap;

	kore_audio_buffer audio_buffer;
}

static bool initialized = false;

void kore_audio_init() {
	if (initialized) {
		return;
	}

	kore_audio_internal_init();
	initialized = true;

	audio_buffer.read_location = 0;
	audio_buffer.write_location = 0;
	audio_buffer.data_size = 128 * 1024;
	audio_buffer.channel_count = 2;
	audio_buffer.channels[0] = new float[audio_buffer.data_size];
	audio_buffer.channels[1] = new float[audio_buffer.data_size];

	kore_microsoft_affirm(DirectSoundCreate8(nullptr, &dsound, nullptr));
	// TODO (DK) only for the main window?
	kore_microsoft_affirm(dsound->SetCooperativeLevel(kore_windows_window_handle(0), DSSCL_PRIORITY));

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = samplesPerSecond;
	waveFormat.wBitsPerSample = bitsPerSample;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC bufferDesc;
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS;
	bufferDesc.dwBufferBytes = dsize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	kore_microsoft_affirm(dsound->CreateSoundBuffer(&bufferDesc, &dbuffer, nullptr));

	DWORD size1;
	uint8_t *buffer1 = NULL;
	kore_microsoft_affirm(dbuffer->Lock(writePos, gap, (void **)&buffer1, &size1, nullptr, nullptr, 0));
	assert(buffer1 != NULL);
	for (DWORD i = 0; i < size1; ++i) {
		buffer1[i] = 0;
	}
	kore_microsoft_affirm(dbuffer->Unlock(buffer1, size1, nullptr, 0));

	kore_microsoft_affirm(dbuffer->Play(0, 0, DSBPLAY_LOOPING));
}

uint32_t kore_audio_samples_per_second(void) {
	return samplesPerSecond;
}

namespace {
	void copySample(uint8_t *buffer, DWORD &index, bool left) {
		float value = *(float *)&audio_buffer.channels[left ? 0 : 1][audio_buffer.read_location];
		if (!left) {
			audio_buffer.read_location += 1;
			if (audio_buffer.read_location >= audio_buffer.data_size) {
				audio_buffer.read_location = 0;
			}
		}
		*(int16_t *)&buffer[index] = static_cast<int16_t>(value * 32767);
		index += 2;
	}
}

void kore_audio_update() {
	DWORD playPosition;
	DWORD writePosition;
	kore_microsoft_affirm(dbuffer->GetCurrentPosition(&playPosition, &writePosition));

	int dif;
	if (writePos >= writePosition) {
		dif = writePos - writePosition;
	}
	else {
		dif = dsize - writePosition + writePos;
	}

	if (dif < gap) {
		return;
	}
	if (writePos + gap >= dsize) {
		if (playPosition >= writePos || playPosition <= gap) {
			return;
		}
		if (writePosition >= writePos || writePosition <= gap) {
			return;
		}
	}
	else {
		if (playPosition >= writePos && playPosition <= writePos + gap) {
			return;
		}
		if (writePosition >= writePos && writePosition <= writePos + gap) {
			return;
		}
	}

	kore_audio_internal_callback(&audio_buffer, (uint32_t)(gap / 4));

	DWORD size1;
	uint8_t *buffer1;
	kore_microsoft_affirm(dbuffer->Lock(writePos, gap, (void **)&buffer1, &size1, NULL, NULL, 0));

	for (DWORD i = 0; i < size1 - (bitsPerSample / 8 - 1);) {
		copySample(buffer1, i, ((writePos + i) / 2) % 2 == 0);
	}
	writePos += size1;

	kore_microsoft_affirm(dbuffer->Unlock(buffer1, size1, NULL, 0));

	if (writePos >= dsize) {
		writePos -= dsize;
	}
}

void kore_audio_shutdown() {
	if (dbuffer != nullptr) {
		dbuffer->Release();
		dbuffer = nullptr;
	}
	if (dsound != nullptr) {
		dsound->Release();
		dsound = nullptr;
	}
}
