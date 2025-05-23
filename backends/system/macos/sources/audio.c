#include <CoreAudio/AudioHardware.h>
#include <CoreServices/CoreServices.h>

#include <kore3/audio/audio.h>
#include <kore3/backend/video.h>
#include <kore3/log.h>

#include <stdio.h>

static kore_internal_video_sound_stream *video = NULL;

void macPlayVideoSoundStream(kore_internal_video_sound_stream *v) {
	video = v;
}

void macStopVideoSoundStream(void) {
	video = NULL;
}

static void affirm(OSStatus err) {
	if (err != kAudioHardwareNoError) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Error: %i\n", err);
	}
}

static bool                        initialized;
static bool                        soundPlaying;
static AudioDeviceID               device;
static UInt32                      deviceBufferSize;
static UInt32                      size;
static AudioStreamBasicDescription deviceFormat;
static AudioObjectPropertyAddress  address;

static AudioDeviceIOProcID theIOProcID = NULL;

static kore_audio_buffer audio_buffer;

static uint32_t samples_per_second = 44100;

uint32_t kore_audio_samples_per_second(void) {
	return samples_per_second;
}

static void copySample(void *buffer) {
	float left_value  = *(float *)&audio_buffer.channels[0][audio_buffer.read_location];
	float right_value = *(float *)&audio_buffer.channels[1][audio_buffer.read_location];
	audio_buffer.read_location += 1;
	if (audio_buffer.read_location >= audio_buffer.data_size) {
		audio_buffer.read_location = 0;
	}
	((float *)buffer)[0] = left_value;
	((float *)buffer)[1] = right_value;
}

static OSStatus appIOProc(AudioDeviceID inDevice, const AudioTimeStamp *inNow, const AudioBufferList *inInputData, const AudioTimeStamp *inInputTime,
                          AudioBufferList *outOutputData, const AudioTimeStamp *inOutputTime, void *userdata) {
	affirm(AudioObjectGetPropertyData(device, &address, 0, NULL, &size, &deviceFormat));
	if (samples_per_second != (int)deviceFormat.mSampleRate) {
		samples_per_second = (int)deviceFormat.mSampleRate;
		kore_audio_internal_sample_rate_callback();
	}
	int num_frames = deviceBufferSize / deviceFormat.mBytesPerFrame;
	kore_audio_internal_callback(&audio_buffer, num_frames);
	float *output = (float *)outOutputData->mBuffers[0].mData;
	for (int i = 0; i < num_frames; ++i) {
		copySample(output);
		output += 2;
	}
	return kAudioHardwareNoError;
}

static bool initialized = false;

void kore_audio_init(void) {
	if (initialized) {
		return;
	}

	kore_audio_internal_init();
	initialized = true;

	audio_buffer.read_location  = 0;
	audio_buffer.write_location = 0;
	audio_buffer.data_size      = 128 * 1024;
	audio_buffer.channel_count  = 2;
	audio_buffer.channels[0]    = (float *)malloc(audio_buffer.data_size * sizeof(float));
	audio_buffer.channels[1]    = (float *)malloc(audio_buffer.data_size * sizeof(float));

	device = kAudioDeviceUnknown;

	initialized = false;

	size              = sizeof(AudioDeviceID);
	address.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
	address.mScope    = kAudioObjectPropertyScopeGlobal;
	address.mElement  = kAudioObjectPropertyElementMaster;
	affirm(AudioObjectGetPropertyData(kAudioObjectSystemObject, &address, 0, NULL, &size, &device));

	size              = sizeof(UInt32);
	address.mSelector = kAudioDevicePropertyBufferSize;
	address.mScope    = kAudioDevicePropertyScopeOutput;
	affirm(AudioObjectGetPropertyData(device, &address, 0, NULL, &size, &deviceBufferSize));

	kore_log(KORE_LOG_LEVEL_INFO, "deviceBufferSize = %i\n", deviceBufferSize);

	size              = sizeof(AudioStreamBasicDescription);
	address.mSelector = kAudioDevicePropertyStreamFormat;
	address.mScope    = kAudioDevicePropertyScopeOutput;

	affirm(AudioObjectGetPropertyData(device, &address, 0, NULL, &size, &deviceFormat));

	if (deviceFormat.mFormatID != kAudioFormatLinearPCM) {
		kore_log(KORE_LOG_LEVEL_ERROR, "mFormatID !=  kAudioFormatLinearPCM\n");
		return;
	}

	if (!(deviceFormat.mFormatFlags & kLinearPCMFormatFlagIsFloat)) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Only works with float format.\n");
		return;
	}

	if (samples_per_second != (int)deviceFormat.mSampleRate) {
		samples_per_second = (int)deviceFormat.mSampleRate;
		kore_audio_internal_sample_rate_callback();
	}

	initialized = true;

	kore_log(KORE_LOG_LEVEL_INFO, "mSampleRate = %g\n", deviceFormat.mSampleRate);
	kore_log(KORE_LOG_LEVEL_INFO, "mFormatFlags = %08X\n", (unsigned int)deviceFormat.mFormatFlags);
	kore_log(KORE_LOG_LEVEL_INFO, "mBytesPerPacket = %d\n", (unsigned int)deviceFormat.mBytesPerPacket);
	kore_log(KORE_LOG_LEVEL_INFO, "mFramesPerPacket = %d\n", (unsigned int)deviceFormat.mFramesPerPacket);
	kore_log(KORE_LOG_LEVEL_INFO, "mChannelsPerFrame = %d\n", (unsigned int)deviceFormat.mChannelsPerFrame);
	kore_log(KORE_LOG_LEVEL_INFO, "mBytesPerFrame = %d\n", (unsigned int)deviceFormat.mBytesPerFrame);
	kore_log(KORE_LOG_LEVEL_INFO, "mBitsPerChannel = %d\n", (unsigned int)deviceFormat.mBitsPerChannel);

	if (soundPlaying)
		return;

	affirm(AudioDeviceCreateIOProcID(device, appIOProc, NULL, &theIOProcID));
	affirm(AudioDeviceStart(device, theIOProcID));

	soundPlaying = true;
}

void kore_audio_update(void) {}

void kore_audio_shutdown(void) {
	if (!initialized)
		return;
	if (!soundPlaying)
		return;

	affirm(AudioDeviceStop(device, theIOProcID));
	affirm(AudioDeviceDestroyIOProcID(device, theIOProcID));

	soundPlaying = false;
}
