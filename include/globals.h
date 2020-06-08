#pragma once
#include "public.sdk/samples/vst/common/logscale.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include <math.h>

#define PI 3.14159265358979323
namespace Steinberg {
namespace MyFirstPlugin {

	struct GlobalParameters {
		Vst::ParamValue eq1f0;
		Vst::ParamValue eq1q;
		Vst::ParamValue eq1g;
		Vst::SampleRate fs;
		double b0;
		double b1;
		double b2;
		double a0;
		double a1;
		double a2;

		Vst::Sample32 previousSamples32[2];
		Vst::Sample64 previousSamples64[2];
		Vst::Sample32 previousInputSample32[2];
		Vst::Sample64 previousInputSample64[2];
	};

	

	
}
}
