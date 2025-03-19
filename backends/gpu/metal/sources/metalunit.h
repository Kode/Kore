#ifndef KORE_METAL_UNIT_HEADER
#define KORE_METAL_UNIT_HEADER

#include <kore3/gpu/device.h>

#include <assert.h>

#import <MetalKit/MTKView.h>

static id<CAMetalDrawable> drawable = nil;

CAMetalLayer *getMetalLayer(void);

#endif
