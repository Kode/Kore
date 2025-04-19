#ifndef KORE_METAL_TEXTURE_STRUCTS_HEADER
#define KORE_METAL_TEXTURE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kore_metal_texture_usage {
	KORE_METAL_TEXTURE_USAGE_SAMPLE      = 0x00010000,
	KORE_METAL_TEXTURE_USAGE_READ_WRITE  = 0x00020000,
	KORE_METAL_TEXTURE_USAGE_FRAMEBUFFER = 0x00040000,
} kore_metal_texture_usage;

typedef struct kore_metal_texture {
	void   *texture;
	uint8_t dimension;
} kore_metal_texture;

#ifdef __cplusplus
}
#endif

#endif
