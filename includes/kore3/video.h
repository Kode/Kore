#ifndef KORE_VIDEO_HEADER
#define KORE_VIDEO_HEADER

#include <kore3/global.h>

#include <kore3/backend/video.h>
#include <kore3/gpu/texture.h>

/*! \file video.h
    \brief Hardware-assisted video decoding support. Actually supported
    formats vary per target-system.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_video {
	kore_video_impl impl;
} kore_video;

/// <summary>
/// Returns the list of video formats which are supported on the current system.
/// </summary>
/// <returns>A NULL-terminated list of strings representing the supported video-formats</returns>
KORE_FUNC const char **kore_video_formats(void);

/// <summary>
/// Prepares a video object based on an encoded video-file.
/// </summary>
KORE_FUNC void kore_video_init(kore_video *video, const char *filename);

/// <summary>
/// Destroys a video object.
/// </summary>
KORE_FUNC void kore_video_destroy(kore_video *video);

/// <summary>
/// Starts playing a video.
/// </summary>
KORE_FUNC void kore_video_play(kore_video *video, bool loop);

/// <summary>
/// Pauses a video.
/// </summary>
KORE_FUNC void kore_video_pause(kore_video *video);

/// <summary>
/// Stops a video which is equivalent to pausing it and setting the position to 0.
/// </summary>
KORE_FUNC void kore_video_stop(kore_video *video);

/// <summary>
/// Gets the width of the video in pixels.
/// </summary>
/// <returns>The width of the video in pixels</returns>
KORE_FUNC int kore_video_width(kore_video *video);

/// <summary>
/// Gets the height of the video in pixels.
/// </summary>
/// <returns>The height of the video in pixels</returns>
KORE_FUNC int kore_video_height(kore_video *video);

/// <summary>
/// Gets the current image of a playing video which can be rendered using any of Kinc's graphics APIs.
/// </summary>
/// <returns>The current image of a playing video</returns>
KORE_FUNC kore_gpu_texture *kore_video_current_image(kore_video *video);

/// <summary>
/// Gets the duration of the video in seconds.
/// </summary>
/// <returns>The duration of the video in seconds</returns>
KORE_FUNC double kore_video_duration(kore_video *video);

/// <summary>
/// Returns the current playback-position of the video in seconds.
/// </summary>
/// <returns>The current playback-position in seconds</returns>
KORE_FUNC double kore_video_position(kore_video *video);

/// <summary>
/// Returns whether the video reached its end.
/// </summary>
/// <returns>the end-state of the video</returns>
KORE_FUNC bool kore_video_finished(kore_video *video);

/// <summary>
/// Returns whether the video is currently paused.
/// </summary>
/// <returns>The current pause state of the video</returns>
KORE_FUNC bool kore_video_paused(kore_video *video);

/// <summary>
/// Call this every frame to update the video. This is not required on all targets but where it's not required the function just does nothing - so please call
/// it.
/// </summary>
KORE_FUNC void kore_video_update(kore_video *video, double time);

#ifdef __cplusplus
}
#endif

#endif
