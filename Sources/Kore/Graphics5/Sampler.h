#pragma once

#include "Graphics.h"

#include <kinc/graphics5/sampler.h>

namespace Kore {
	namespace Graphics5 {
		struct SamplerOptions {
			TextureAddressing uAddressing;
			TextureAddressing vAddressing;
			TextureAddressing wAddressing;

			TextureFilter minificationFilter;
			TextureFilter magnificationFilter;
			MipmapFilter mipmapFilter;

			float lodMinClamp;
			float lodMaxClamp;

			uint16_t maxAnisotropy;

			bool isComparison;
			CompareMode compareMode;

			SamplerOptions();
		};

		class Sampler {
		public:
			Sampler(SamplerOptions *options);
			~Sampler();

			kinc_g5_sampler_t kincSampler;
		};
	}
}
