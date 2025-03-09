#ifndef KORE_GPU_GRAPHICS_HEADER
#define KORE_GPU_GRAPHICS_HEADER

#include <kore3/global.h>

#include "api.h"

/*! \file gpu.h
    \brief Provides just kore_gpu_init which has to be called to initialize the GPU API.
*/

#ifdef __cplusplus
extern "C" {
#endif

KORE_FUNC void kore_gpu_init(kore_gpu_api api);

#ifdef __cplusplus
}
#endif

#endif
