#ifndef KORE_OPENGL_SAMPLER_STRUCTS_HEADER
#define KORE_OPENGL_SAMPLER_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_opengl_sampler {
	unsigned address_mode_u;
	unsigned address_mode_v;
	unsigned address_mode_w;
	unsigned min_filter;
	unsigned mag_filter;
	unsigned mip_filter;
	float    lod_min_clamp;
	float    lod_max_clamp;
	unsigned compare;
	uint16_t max_anisotropy;
} kore_opengl_sampler;

#ifdef __cplusplus
}
#endif

#endif
