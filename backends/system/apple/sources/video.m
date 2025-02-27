#include <kore3/video.h>

#include <kore3/gpu/texture.h>
#include <kore3/io/filereader.h>
#include <kore3/log.h>
#include <kore3/mixer/mixer.h>
#include <kore3/system.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#import <AVFoundation/AVFoundation.h>

extern const char *iphonegetresourcepath(void);
extern const char *macgetresourcepath(void);

void kore_internal_video_sound_stream_init(kore_internal_video_sound_stream *stream, int channel_count, int frequency) {
	stream->bufferSize = 1024 * 100;
	stream->bufferReadPosition = 0;
	stream->bufferWritePosition = 0;
	stream->read = 0;
	stream->written = 0;
	stream->buffer = (float *)malloc(stream->bufferSize * sizeof(float));
}

void kore_internal_video_sound_stream_destroy(kore_internal_video_sound_stream *stream) {
	free(stream->buffer);
}

void kore_internal_video_sound_stream_insert_data(kore_internal_video_sound_stream *stream, float *data, int sample_count) {
	for (int i = 0; i < sample_count; ++i) {
		float value = data[i]; // / 32767.0;
		stream->buffer[stream->bufferWritePosition++] = value;
		++stream->written;
		if (stream->bufferWritePosition >= stream->bufferSize) {
			stream->bufferWritePosition = 0;
		}
	}
}

static float samples[2] = {0};

float *kore_internal_video_sound_stream_next_frame(kore_internal_video_sound_stream *stream) {
	++stream->read;
	if (stream->written <= stream->read) {
		kore_log(KORE_LOG_LEVEL_WARNING, "Out of audio\n");
		return 0;
	}

	if (stream->bufferReadPosition >= stream->bufferSize) {
		stream->bufferReadPosition = 0;
		kore_log(KORE_LOG_LEVEL_INFO, "buffer read back - %i\n", (int)(stream->written - stream->read));
	}
	samples[0] = stream->buffer[stream->bufferReadPosition++];

	if (stream->bufferReadPosition >= stream->bufferSize) {
		stream->bufferReadPosition = 0;
		kore_log(KORE_LOG_LEVEL_INFO, "buffer read back - %i\n", (int)(stream->written - stream->read));
	}
	samples[1] = stream->buffer[stream->bufferReadPosition++];

	return samples;
}

bool kore_internal_video_sound_stream_ended(kore_internal_video_sound_stream *stream) {
	return false;
}

static void load(kore_video *video, double startTime) {
	video->impl.videoStart = startTime;
	AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:video->impl.url options:nil];
	video->impl.videoAsset = asset;

	video->impl.duration = [asset duration].value / [asset duration].timescale;

	AVAssetTrack *videoTrack = [[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
	NSDictionary *videoOutputSettings =
	    [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey, nil];
	AVAssetReaderTrackOutput *videoOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:videoTrack outputSettings:videoOutputSettings];
	[videoOutput setSupportsRandomAccess:YES];

	bool hasAudio = [[asset tracksWithMediaType:AVMediaTypeAudio] count] > 0;
	AVAssetReaderAudioMixOutput *audioOutput = NULL;
	if (hasAudio) {
		AVAssetTrack *audioTrack = [[asset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];
		NSDictionary *audioOutputSettings = [NSDictionary
		    dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kAudioFormatLinearPCM], AVFormatIDKey, [NSNumber numberWithFloat:44100.0], AVSampleRateKey,
		                                 [NSNumber numberWithInt:32], AVLinearPCMBitDepthKey, [NSNumber numberWithBool:NO], AVLinearPCMIsNonInterleaved,
		                                 [NSNumber numberWithBool:YES], AVLinearPCMIsFloatKey, [NSNumber numberWithBool:NO], AVLinearPCMIsBigEndianKey, nil];
		audioOutput = [AVAssetReaderAudioMixOutput assetReaderAudioMixOutputWithAudioTracks:@[ audioTrack ] audioSettings:audioOutputSettings];
		[audioOutput setSupportsRandomAccess:YES];
	}

	AVAssetReader *reader = [AVAssetReader assetReaderWithAsset:asset error:nil];

	if (startTime > 0) {
		CMTimeRange timeRange = CMTimeRangeMake(CMTimeMake(startTime * 1000, 1000), kCMTimePositiveInfinity);
		reader.timeRange = timeRange;
	}

	[reader addOutput:videoOutput];
	if (hasAudio) {
		[reader addOutput:audioOutput];
	}

	video->impl.assetReader = reader;
	video->impl.videoTrackOutput = videoOutput;
	if (hasAudio) {
		video->impl.audioTrackOutput = audioOutput;
	}
	else {
		video->impl.audioTrackOutput = NULL;
	}

	if (video->impl.myWidth < 0)
		video->impl.myWidth = [videoTrack naturalSize].width;
	if (video->impl.myHeight < 0)
		video->impl.myHeight = [videoTrack naturalSize].height;
	int framerate = [videoTrack nominalFrameRate];
	kore_log(KORE_LOG_LEVEL_INFO, "Framerate: %i\n", framerate);
	video->impl.next = video->impl.videoStart;
	video->impl.audioTime = video->impl.videoStart * 44100;
}

void kore_video_init(kore_video *video, const char *filename) {
	video->impl.playing = false;
	video->impl.sound = NULL;
	video->impl.image_initialized = false;
	char name[2048];
#ifdef KORE_IOS
	strcpy(name, iphonegetresourcepath());
#else
	strcpy(name, macgetresourcepath());
#endif
	strcat(name, "/");
	strcat(name, KORE_DEBUGDIR);
	strcat(name, "/");
	strcat(name, filename);
	video->impl.url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:name]];
	video->impl.myWidth = -1;
	video->impl.myHeight = -1;
	video->impl.finished = false;
	video->impl.duration = 0;
	load(video, 0);
}

void kore_video_destroy(kore_video *video) {
	kore_video_stop(video);
}

#ifdef KORE_IOS
void iosPlayVideoSoundStream(kore_internal_video_sound_stream *video);
void iosStopVideoSoundStream(void);
#else
void macPlayVideoSoundStream(kore_internal_video_sound_stream *video);
void macStopVideoSoundStream(void);
#endif

void kore_video_play(kore_video *video, bool loop) {
	AVAssetReader *reader = video->impl.assetReader;
	[reader startReading];

	kore_internal_video_sound_stream *stream = (kore_internal_video_sound_stream *)malloc(sizeof(kore_internal_video_sound_stream));
	kore_internal_video_sound_stream_init(stream, 2, 44100);
	video->impl.sound = stream;
#ifdef KORE_IOS
	iosPlayVideoSoundStream((kore_internal_video_sound_stream *)video->impl.sound);
#else
	macPlayVideoSoundStream((kore_internal_video_sound_stream *)video->impl.sound);
#endif

	video->impl.playing = true;
	video->impl.start = kore_time() - video->impl.videoStart;
	video->impl.loop = loop;
}

void kore_video_pause(kore_video *video) {
	video->impl.playing = false;
	if (video->impl.sound != NULL) {
// Mixer::stop(sound);
#ifdef KORE_IOS
		iosStopVideoSoundStream();
#else
		macStopVideoSoundStream();
#endif
		kore_internal_video_sound_stream_destroy((kore_internal_video_sound_stream *)video->impl.sound);
		free(video->impl.sound);
		video->impl.sound = NULL;
	}
}

void kore_video_stop(kore_video *video) {
	kore_video_pause(video);
	video->impl.finished = true;
}

static void updateImage(kore_video *video) {
	if (!video->impl.playing)
		return;

	{
		AVAssetReaderTrackOutput *videoOutput = video->impl.videoTrackOutput;
		CMSampleBufferRef buffer = [videoOutput copyNextSampleBuffer];
		if (!buffer) {
			if (video->impl.loop) {
				CMTimeRange timeRange = CMTimeRangeMake(CMTimeMake(0, 1000), kCMTimePositiveInfinity);
				[videoOutput resetForReadingTimeRanges:[NSArray arrayWithObject:[NSValue valueWithCMTimeRange:timeRange]]];

				AVAssetReaderAudioMixOutput *audioOutput = video->impl.audioTrackOutput;
				CMSampleBufferRef audio_buffer = [audioOutput copyNextSampleBuffer];
				while (audio_buffer) {
					audio_buffer = [audioOutput copyNextSampleBuffer];
				}
				[audioOutput resetForReadingTimeRanges:[NSArray arrayWithObject:[NSValue valueWithCMTimeRange:timeRange]]];

				buffer = [videoOutput copyNextSampleBuffer];

				video->impl.start = kore_time() - video->impl.videoStart;
			}
			else {
				kore_video_stop(video);
				return;
			}
		}
		video->impl.next = CMTimeGetSeconds(CMSampleBufferGetOutputPresentationTimeStamp(buffer));

		CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(buffer);

		if (!video->impl.image_initialized) {
			CGSize size = CVImageBufferGetDisplaySize(pixelBuffer);
			video->impl.myWidth = size.width;
			video->impl.myHeight = size.height;
			// kore_g4_texture_init(&video->impl.image, kore_video_width(video), kore_video_height(video), KORE_IMAGE_FORMAT_BGRA32); // TODO
			video->impl.image_initialized = true;
		}

		if (pixelBuffer != NULL) {
			CVPixelBufferLockBaseAddress(pixelBuffer, 0);
#ifdef KORE_OPENGL
			// TODO
			// kore_g4_texture_upload(&video->impl.image, (uint8_t *)CVPixelBufferGetBaseAddress(pixelBuffer),
			//                       (int)(CVPixelBufferGetBytesPerRow(pixelBuffer) / 4));
#else
			// TODO
			// kore_g4_texture_upload(&video->impl.image, (uint8_t *)CVPixelBufferGetBaseAddress(pixelBuffer), (int)(CVPixelBufferGetBytesPerRow(pixelBuffer)));
#endif
			CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
		}
		CFRelease(buffer);
	}

	if (video->impl.audioTrackOutput != NULL) {
		AVAssetReaderAudioMixOutput *audioOutput = video->impl.audioTrackOutput;
		while (video->impl.audioTime / 44100.0 < video->impl.next + 0.1) {
			CMSampleBufferRef buffer = [audioOutput copyNextSampleBuffer];
			if (!buffer)
				return;
			CMItemCount numSamplesInBuffer = CMSampleBufferGetNumSamples(buffer);
			AudioBufferList audioBufferList;
			CMBlockBufferRef blockBufferOut = nil;
			CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(buffer, NULL, &audioBufferList, sizeof(audioBufferList), NULL, NULL,
			                                                        kCMSampleBufferFlag_AudioBufferList_Assure16ByteAlignment, &blockBufferOut);
			for (int bufferCount = 0; bufferCount < audioBufferList.mNumberBuffers; ++bufferCount) {
				float *samples = (float *)audioBufferList.mBuffers[bufferCount].mData;
				kore_internal_video_sound_stream *sound = (kore_internal_video_sound_stream *)video->impl.sound;
				if (video->impl.audioTime / 44100.0 > video->impl.next - 0.1) {
					kore_internal_video_sound_stream_insert_data(sound, samples, (int)numSamplesInBuffer * 2);
				}
				else {
					// Send some data anyway because the buffers are huge
					kore_internal_video_sound_stream_insert_data(sound, samples, (int)numSamplesInBuffer);
				}
				video->impl.audioTime += numSamplesInBuffer;
			}
			CFRelease(blockBufferOut);
			CFRelease(buffer);
		}
	}
}

void kore_video_update(kore_video *video, double time) {
	if (video->impl.playing && time >= video->impl.start + video->impl.next) {
		updateImage(video);
	}
}

int kore_video_width(kore_video *video) {
	return video->impl.myWidth;
}

int kore_video_height(kore_video *video) {
	return video->impl.myHeight;
}

kore_gpu_texture *kore_video_current_image(kore_video *video) {
	kore_video_update(video, kore_time());
	return &video->impl.image;
}

double kore_video_duration(kore_video *video) {
	return video->impl.duration;
}

bool kore_video_finished(kore_video *video) {
	return video->impl.finished;
}

bool kore_video_paused(kore_video *video) {
	return !video->impl.playing;
}

double kore_video_position(kore_video *video) {
	return video->impl.next - video->impl.start;
}
