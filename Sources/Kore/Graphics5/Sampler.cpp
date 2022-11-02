#include "Sampler.h"

#include <kinc/graphics5/sampler.h>

using namespace Kore::Graphics5;

SamplerOptions::SamplerOptions() {
	kinc_g5_sampler_options kincOptions;
	kinc_g5_sampler_options_set_defaults(&kincOptions);

	uAddressing = (TextureAddressing)kincOptions.u_addressing;
	vAddressing = (TextureAddressing)kincOptions.v_addressing;
	wAddressing = (TextureAddressing)kincOptions.w_addressing;

	minificationFilter = (TextureFilter)kincOptions.minification_filter;
	magnificationFilter = (TextureFilter)kincOptions.magnification_filter;
	mipmapFilter = (MipmapFilter)kincOptions.mipmap_filter;

	lodMinClamp = kincOptions.lod_min_clamp;
	lodMaxClamp = kincOptions.lod_max_clamp;

	maxAnisotropy = kincOptions.max_anisotropy;

	isComparison = kincOptions.is_comparison;
	compareMode = (CompareMode)kincOptions.compare_mode;
}

Sampler::Sampler(SamplerOptions *options) {
	kinc_g5_sampler_options kincOptions;
	kinc_g5_sampler_options_set_defaults(&kincOptions);

	kincOptions.u_addressing = (kinc_g5_texture_addressing_t)options->uAddressing;
	kincOptions.v_addressing = (kinc_g5_texture_addressing_t)options->vAddressing;
	kincOptions.w_addressing = (kinc_g5_texture_addressing_t)options->wAddressing;

	kincOptions.minification_filter = (kinc_g5_texture_filter_t)options->minificationFilter;
	kincOptions.magnification_filter = (kinc_g5_texture_filter_t)options->magnificationFilter;
	kincOptions.mipmap_filter = (kinc_g5_mipmap_filter_t)options->mipmapFilter;

	kincOptions.lod_min_clamp = options->lodMinClamp;
	kincOptions.lod_max_clamp = options->lodMaxClamp;

	kincOptions.max_anisotropy = options->maxAnisotropy;

	kincOptions.is_comparison = options->isComparison;
	kincOptions.compare_mode = (kinc_g5_compare_mode_t)options->compareMode;

	kinc_g5_sampler_init(&kincSampler, &kincOptions);
}

Sampler::~Sampler() {
	kinc_g5_sampler_destroy(&kincSampler);
}
