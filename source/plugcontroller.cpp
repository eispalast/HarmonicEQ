//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : plugcontroller.cpp
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

#include "../include/plugcontroller.h"
#include "../include/plugids.h"
#include "../include/globals.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"


using namespace VSTGUI;

namespace Steinberg {
namespace MyFirstPlugin {

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugController::initialize (FUnknown* context)
{
	tresult result = EditController::initialize (context);
	if (result == kResultTrue)
	{
		using namespace Vst;
		//---Create Parameters------------
		parameters.addParameter (STR16 ("Bypass"), nullptr, 1, 0,
		                         Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass,
		                         MyFirstPluginParams::kBypassId);

		parameters.addParameter (STR16 ("Master Volume"), STR16 ("dB"), 0, .9,
		                         Vst::ParameterInfo::kCanAutomate, MyFirstPluginParams::kParamVolId, 0,
		                         STR16 ("Vol1"));
		Parameter* param;
		LogScale<ParamValue>* logscale =new LogScale<ParamValue>(0., 1., 5., 20000., 0.5, 1000.);
		LogScale<ParamValue>* qLogscale = new LogScale<ParamValue>(0., 1., 0.05, 20., 0.5, 4.);
		
		//Band 1 has fundamental frequency, the other band's freqs are calculated from the fundamental frequency but can be modified with an offset
		param = new LogScaleParameter<ParamValue> (STR16 ("Eq1 f"), kParamEq1f0, *logscale,STR16("Hz"));
		param->setPrecision(1);
		param->setNormalized(logscale->invscale(80));
		parameters.addParameter(param);

		param = new RangeParameter(STR16("Eq1 Offset"), kParamEq1o, STR16(""), -0.9, 0.9, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//param = new RangeParameter(STR16("Eq1 q"), kParamEq1q, STR16(""), 0.05, 20, 3);
		param = new LogScaleParameter<ParamValue>(STR16("Eq1 q"), kParamEq1q, *qLogscale, STR16("q"));
		param->setNormalized(qLogscale->invscale(4));
		param->setPrecision(2);
		parameters.addParameter(param);

		param = new RangeParameter(STR16("Eq1 gain"), kParamEq1g, STR16("dB"), -20, 20, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//Band 2
		param = new RangeParameter(STR16("Eq2 Offset"), kParamEq2o, STR16(""), -0.5, 0.5, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//param = new RangeParameter(STR16("Eq2 q"), kParamEq2q, STR16(""), 0.05, 20, 3);
		param = new LogScaleParameter<ParamValue>(STR16("Eq2 q"), kParamEq2q, *qLogscale, STR16("q"));
		param->setNormalized(qLogscale->invscale(4));
		param->setPrecision(2);
		parameters.addParameter(param);

		param = new RangeParameter(STR16("Eq2 gain"), kParamEq2g, STR16("dB"), -20, 20, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//Band 3
		param = new RangeParameter(STR16("Eq3 Offset"), kParamEq3o, STR16(""), -0.34, 0.34, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//param = new RangeParameter(STR16("Eq3 q"), kParamEq3q, STR16(""), 0.05, 20, 3);
		param = new LogScaleParameter<ParamValue>(STR16("Eq3 q"), kParamEq3q, *qLogscale, STR16("q"));
		param->setNormalized(qLogscale->invscale(4));
		param->setPrecision(2);
		parameters.addParameter(param);

		param = new RangeParameter(STR16("Eq3 gain"), kParamEq3g, STR16("dB"), -20, 20, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//Band 4
		param = new RangeParameter(STR16("Eq4 Offset"), kParamEq4o, STR16(""), -0.25, 0.25, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//param = new RangeParameter(STR16("Eq4 q"), kParamEq4q, STR16(""), 0.05, 20, 3);
		param = new LogScaleParameter<ParamValue>(STR16("Eq4 q"), kParamEq4q, *qLogscale, STR16("q"));
		param->setNormalized(qLogscale->invscale(4));
		param->setPrecision(2);
		parameters.addParameter(param);

		param = new RangeParameter(STR16("Eq4 gain"), kParamEq4g, STR16("dB"), -20, 20, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//Band 5
		param = new RangeParameter(STR16("Eq5 Offset"), kParamEq5o, STR16(""), -0.2, 0.2, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//param = new RangeParameter(STR16("Eq5 q"), kParamEq5q, STR16(""), 0.05, 20, 3);
		param = new LogScaleParameter<ParamValue>(STR16("Eq5 q"), kParamEq5q, *qLogscale, STR16("q"));
		param->setNormalized(qLogscale->invscale(4));
		param->setPrecision(2);
		parameters.addParameter(param);

		param = new RangeParameter(STR16("Eq5 gain"), kParamEq5g, STR16("dB"), -20, 20, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//Band 6
		param = new RangeParameter(STR16("Eq6 Offset"), kParamEq6o, STR16(""), -0.17, 0.17, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		//param = new RangeParameter(STR16("Eq6 q"), kParamEq6q, STR16(""), 0.05, 20, 3);
		param = new LogScaleParameter<ParamValue>(STR16("Eq6 q"), kParamEq6q, *qLogscale, STR16("q"));
		param->setNormalized(qLogscale->invscale(4));
		param->setPrecision(2);
		parameters.addParameter(param);

		param = new RangeParameter(STR16("Eq6 gain"), kParamEq6g, STR16("dB"), -20, 20, 0);
		param->setPrecision(2);
		parameters.addParameter(param);

		
	}
	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API PlugController::createView (const char* name)
{
	// someone wants my editor
	if (name && strcmp (name, "editor") == 0)
	{
		auto* view = new VST3Editor (this, "view", "plug.uidesc");
		return view;
	}
	return nullptr;
}
/*

IPlugView* PLUGIN_API PlugController::createView(FIDString name)
{
	if (strcmp(name, Vst::ViewType::kEditor) == 0)
	{
		return new VSTGUI::VST3Editor(this, "view", "myEditor.uidesc");
	}
	return 0;
}
*/




//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::setComponentState (IBStream* state)
{
	// we receive the current state of the component (processor part)
	// we read our parameters and bypass value...
	if (!state)
		return kResultFalse;

	IBStreamer streamer (state, kLittleEndian);

	float savedParam1 = 0.f;
	if (streamer.readFloat (savedParam1) == false)
		return kResultFalse;
	setParamNormalized (MyFirstPluginParams::kParamVolId, savedParam1);

	int8 savedParam2 = 0;
	if (streamer.readInt8 (savedParam2) == false)
		return kResultFalse;
	setParamNormalized (MyFirstPluginParams::kParamOnId, savedParam2);

	// read the bypass
	int32 bypassState;
	if (streamer.readInt32 (bypassState) == false)
		return kResultFalse;
	setParamNormalized (kBypassId, bypassState ? 1 : 0);

	return kResultOk;
}



//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
