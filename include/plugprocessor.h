//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : plugprocessor.h
// Created by  : Steinberg, 01/2018
// Description : MyFirstPlugin Example for VST 3
//
//-----------------------------------------------------------------------------
// LICENSE
// (c) 2019, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include <iostream>
#include <fstream>
#include "globals.h"

namespace Steinberg {
namespace MyFirstPlugin {

//-----------------------------------------------------------------------------
	class PlugProcessor : public Vst::AudioEffect
	{
	public:
		PlugProcessor();
		//calc all params
		tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
		
		void calc_params() {
			bool f0[6] = { true,true, true, true, true, true };
			bool g[6] = { true,true, true, true, true, true };
			bool q[6] = { true,true, true, true, true, true };
			calc_params(f0, g, q);
		}

		//only recalculate specific parameters
		void calc_params(bool f0[6], bool g[6], bool q[6]) {
			bool debug = false;

			if (debug) myfile.open("G:\VSTPluginOutput2.txt", std::ios::app);
			if (debug)myfile << "In calc params\n";
			for (int i = 0; i < 6; i++) {

				if (g[i]) {
					A[i] = pow(10, (eqg[i] / 40.0));
					if (debug)myfile << "\n Changed A value for band " << i;
				}
				if (f0[i]) {
					if (debug)myfile << "\n Changed omega, cos, sin, alpha value for band " << i;
					//TODO: Watch out for frequencies above 20kHz
					double omega = (double)(2 * PI * ((eqf0[i]+eqf0[i]*freqOffset[i]) / fs));
					if (debug)myfile << "\n Omega: " << omega << "  eqf0: "<< eqf0[i] << " i: " << i;
					cos_omega[i] = (double)cos(omega);
					sin_omega[i] = (double)sin(omega);
					alpha[i] = (sin_omega[i]) / (2 * eqq[i]);
				}
				if (q[i]) {
					if (debug)myfile << "\n Changed alpha value for band " << i;
					alpha[i] = (sin_omega[i]) / (2 * eqq[i]);
				}

				if (g[i] || q[i]) {
					if (debug)myfile << "\n Changed b0b2a0a2 value for band " << i;
					b0[i] = 1 + alpha[i] * A[i];
					b2[i] = 1 - alpha[i] * A[i];
					a0[i] = 1 + (alpha[i] / A[i]);
					a2[i] = 1 - (alpha[i] / A[i]);
				}
				if (f0[i]) {
					if (debug)myfile << "\n Changed b1 a1 value for band " << i;
					b1[i] = -2 * cos_omega[i];
					a1[i] = -2 * cos_omega[i];
				}
			}
			if (debug) {
				myfile << "Set Params:"
					<< "\nfs: " << fs
					<< "\neq1f0 " << eqf0[0] << "  eq2f0: " << eqf0[1]
					<< "\neq1q " << eqq[0] << "  eq2q: " << eqq[1]
					<< "\neq1g " << eqg[0] << "  eq2g: " << eqg[1];

				myfile << "\nCalced Params:"
					<< "\n a0 1: " << a0[0] << "  a0 2: " << a0[1]
					<< "\n a1 1: " << a1[0] << "  a1 2: " << a1[1]
					<< "\n a2 1: " << a2[0] << "  a2 2: " << a2[1]
					<< "\n b0 1: " << b0[0] << "  b0 2: " << b0[1]
					<< "\n b1 1: " << b1[0] << "  b1 2: " << b1[1]
					<< "\n b2 1: " << b2[0] << "  b2 2: " << b2[1];
				myfile.close();
			}
		}
	tresult PLUGIN_API setBusArrangements (Vst::SpeakerArrangement* inputs, int32 numIns,
	                                       Vst::SpeakerArrangement* outputs,
	                                       int32 numOuts) SMTG_OVERRIDE;

	tresult PLUGIN_API setupProcessing (Vst::ProcessSetup& setup) SMTG_OVERRIDE;
	tresult PLUGIN_API setActive (TBool state) SMTG_OVERRIDE;
	tresult PLUGIN_API process (Vst::ProcessData& data) SMTG_OVERRIDE;

//------------------------------------------------------------------------
	tresult PLUGIN_API setState (IBStream* state) SMTG_OVERRIDE;
	tresult PLUGIN_API getState (IBStream* state) SMTG_OVERRIDE;

	static FUnknown* createInstance (void*) { return (Vst::IAudioProcessor*)new PlugProcessor (); }

protected:
	Vst::ParamValue mParam1 = 0;
	int16 mParam2 = 0;
	bool mBypass = false;
	Vst::ParamValue eqf0[6] = { 80,160,240,320,400,480 };
	Vst::ParamValue eqq[6] = { 3,3,3,3,3,3 };
	Vst::ParamValue eqg[6] = { 0,0,0,0,0,0 };
	Vst::SampleRate fs=44100;
	double sin_omega[6];
	double cos_omega[6];
	double alpha[6];
	double freqOffset[6] = { 0,0,0,0,0,0 }; 
	double A[6];
	double a0[6]= {1.3685761281637796, 0,0,0,0,0};
	double a1[6]= {-1.8200702223285332,0,0,0,0,0};
	double a2[6]= {0.6314238718362205, 0,0,0,0,0};
	double b0[6]= {1.1165540056163676, 0,0,0,0,0};
	double b1[6]= {-1.8200702223285332,0,0,0,0,0};
	double b2[6]= {0.8834459943836322, 0,0,0,0,0};

	//[Band][Maximum number of channels][last/secondlast Sample]
	Vst::Sample32 previousSamples32[6][10][2];
	Vst::Sample64 previousSamples64[6][10][2];
	Vst::Sample32 previousInputSample32[6][10][2];
	Vst::Sample64 previousInputSample64[6][10][2];

	Vst::LogScale<Vst::ParamValue>* freqLogscale = new Vst::LogScale<Vst::ParamValue>(0., 1., 5., 20000., 0.5, 1000.);
	Vst::LogScale<Vst::ParamValue>* qLogscale = new Vst::LogScale<Vst::ParamValue>(0., 1., 0.05, 20., 0.5, 4.);

	Vst::RangeParameter* qRange = new Vst::RangeParameter(STR16("qRange"), NULL, STR16(""), 0.05, 20, 3);
	Vst::RangeParameter* gRange = new Vst::RangeParameter(STR16("gRange"), NULL, STR16("dB"), -20, 20, 0);
	Vst::RangeParameter* oRange0 = new Vst::RangeParameter(STR16("Offset0"), NULL, STR16(""), -0.9, 0.9, 0);
	Vst::RangeParameter* oRange1 = new Vst::RangeParameter(STR16("Offset1"), NULL, STR16(""), -0.5, 0.5, 0);
	Vst::RangeParameter* oRange2 = new Vst::RangeParameter(STR16("Offset2"), NULL, STR16(""), -0.34, 0.34, 0);
	Vst::RangeParameter* oRange3 = new Vst::RangeParameter(STR16("Offset3"), NULL, STR16(""), -0.25, 0.25, 0);
	Vst::RangeParameter* oRange4 = new Vst::RangeParameter(STR16("Offset4"), NULL, STR16(""), -0.2, 0.2, 0);
	Vst::RangeParameter* oRange5 = new Vst::RangeParameter(STR16("Offset5"), NULL, STR16(""), -0.17, 0.17, 0);

	std::ofstream myfile;
};

//------------------------------------------------------------------------
} // namespace MyFirstPlugin
} // namespace Steinberg
