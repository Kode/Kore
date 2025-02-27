#ifndef KORE_ANDROID_HEADER
#define KORE_ANDROID_HEADER

#include "android_native_app_glue.h"

#ifdef __cplusplus
extern "C" {
#endif

// name in usual Java syntax (points, no slashes)
jclass kore_android_find_class(JNIEnv *env, const char *name);

ANativeActivity *kore_android_get_activity(void);

AAssetManager *kore_android_get_asset_manager(void);

#ifdef __cplusplus
}
#endif

#endif
