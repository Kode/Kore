#ifndef KORE_VR_HEADER
#define KORE_VR_HEADER

#include <kinc/global.h>

#include <kore3/math/matrix.h>
#include <kore3/math/quaternion.h>
#include <kore3/math/vector.h>

/*! \file vrinterface.h
    \brief The C-API for VR is currently deactivated and needs some work. Please send pull-requests.
*/

#ifdef KORE_VR

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { KORE_TRACKING_ORIGIN_STAND, KORE_TRACKING_ORIGIN_SIT } kore_tracking_origin;

typedef enum { KORE_TRACKED_DEVICE_HMD, KORE_TRACKED_DEVICE_CONTROLLER, KORE_TRACKED_DEVICE_VIVE_TRACKER } kore_tracked_device;

typedef struct kore_vr_pose {
	kore_quaternion orientation;
	kore_float3 position;

	kore_matrix4x4 eye;
	kore_matrix4x4 projection;

	// fov
	float left;
	float right;
	float bottom;
	float top;
} kore_vr_pose;

typedef struct kore_vr_pose_state {
	kore_vr_pose vrPose;
	kore_float3 angularVelocity;     // Angular velocity in radians per second
	kore_float3 linearVelocity;      // Velocity in meters per second
	kore_float3 angularAcceleration; // Angular acceleration in radians per second per second
	kore_float3 linearAcceleration;  // Acceleration in meters per second per second

	kinc_tracked_device_t trackedDevice;

	// Sensor status
	bool isVisible;
	bool hmdPresenting;
	bool hmdMounted;
	bool displayLost;
	bool shouldQuit;
	bool shouldRecenter;
} kore_vr_pose_state;

typedef struct kore_vr_sensor_state {
	kore_vr_pose_state pose;
} kore_vr_sensor_state;

void *kore_vr_init(void *hinst, const char *title, const char *windowClassName);
void kore_vr_begin();
void kore_vr_begin_render(int eye);
void kore_vr_end_render(int eye);
kore_vr_sensor_state kore_vr_get_sensor_state(int eye);
kore_vr_pose_state kore_vr_get_controller(int index);
void kore_vr_warp_swap();
void kore_vr_update_tracking_origin(kore_tracking_origin origin);
void kore_vr_reset_hmd_pose();
void kore_vr_ovr_shutdown();

#ifdef __cplusplus
}
#endif

#endif

#endif
