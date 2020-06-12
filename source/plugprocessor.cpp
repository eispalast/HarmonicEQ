//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : plugprocessor.cpp
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

#include "../include/plugprocessor.h"
#include "../include/plugids.h"
#include "../include/globals.h"
#include "../include/plugcontroller.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

namespace Steinberg {
namespace MyFirstPlugin {

//-----------------------------------------------------------------------------
PlugProcessor::PlugProcessor ()
{
	// register its editor class
	
	
	setControllerClass (MyControllerUID);
}



//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::initialize (FUnknown* context)
{
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	if (result != kResultTrue)
		return kResultFalse;

	//---create Audio In/Out buses------
	// we want a stereo Input and a Stereo Output
	addAudioInput (STR16 ("AudioInput"), Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("AudioOutput"), Vst::SpeakerArr::kStereo);
	//myfile.open("G:\VSTPluginOutput.txt", std::ios::app);
	//myfile << "\n--------------\nInitialisiert\n";
	//myfile.close();

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 6; k++) {
				previousSamples32[k][i][j] = 0;
				previousSamples64[k][i][j] = 0;
				previousInputSample32[k][i][j] = 0;
				previousInputSample64[k][i][j] = 0;
			}

		}
	}


	PlugProcessor::calc_params();
	return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setBusArrangements (Vst::SpeakerArrangement* inputs,
                                                            int32 numIns,
                                                            Vst::SpeakerArrangement* outputs,
                                                            int32 numOuts)
{
	// we only support one in and output bus and these buses must have the same number of channels
	if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0])
	{
		return AudioEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setupProcessing (Vst::ProcessSetup& setup)
{
	// here you get, with setup, information about:
	// sampleRate, processMode, maximum number of samples per audio block
	fs = setup.sampleRate;
	//myfile.open("G:\VSTPluginOutput.txt", std::ios::app);
	//myfile << "Doing SetupProcessing\n";
	//myfile.close();
	//PlugProcessor::calc_params();
	return AudioEffect::setupProcessing (setup);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setActive (TBool state)
{
	if (state) // Initialize
	{
		// Allocate Memory Here
		// Ex: algo.create ();
	}
	else // Release
	{
		// Free Memory if still allocated
		// Ex: if(algo.isCreated ()) { algo.destroy (); }
	}
	//Initialize the globals or else no calculation can be done
	
	
	//myfile.open("G:\VSTPluginOutput.txt", std::ios::app);
	//myfile << "Set Active\n";
	//myfile.close();
	//PlugProcessor::calc_params();
	return AudioEffect::setActive (state);
}




//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::process (Vst::ProcessData& data)
{
	//--- Read inputs parameter changes-----------
	if (data.inputParameterChanges)
	{
		bool q[6] =	{false,false,false,false,false,false};
		bool f0[6] ={false,false,false,false,false,false};
		bool g[6] =	{false,false,false,false,false,false};
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			Vst::IParamValueQueue* paramQueue =
			    data.inputParameterChanges->getParameterData (index);
			if (paramQueue)
			{
				
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount ();
				switch (paramQueue->getParameterId ())
				{
					case MyFirstPluginParams::kParamVolId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
						    kResultTrue)
							mParam1 = value;
						break;
					case MyFirstPluginParams::kParamOnId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
						    kResultTrue)
							mParam2 = value > 0 ? 1 : 0;
						break;
					case MyFirstPluginParams::kBypassId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
						    kResultTrue)
							mBypass = (value > 0.5f);
						break;
					//Band 1
					case MyFirstPluginParams::kParamEq1f0:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							eqf0[0] = freqLogscale->scale(value);
							//TODO: Watch out for frequencies above 20kHz
							for (int32 i = 0; i < 6; i++) {
								eqf0[i] = eqf0[0] * ((double)i + (double)1);
								f0[i] = true;
							}
							
						}
						break;

					case MyFirstPluginParams::kParamEq1o:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							freqOffset[0] = oRange0->toPlain(value);
							f0[0] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq1q:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							//eqq[0] = qRange->toPlain(value);
							eqq[0] = qLogscale->scale(value);
							q[0] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq1g:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							eqg[0] = gRange->toPlain(value);
							g[0] = true;
						}
					break;
					//Band 2
					case MyFirstPluginParams::kParamEq2o:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							freqOffset[1] = oRange1->toPlain(value);
							f0[1] = true;
						}
					break;
					case MyFirstPluginParams::kParamEq2q:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							//eqq[1] = qRange->toPlain(value);
							eqq[1] = qLogscale->scale(value);
							q[1] = true;
						}
					break;
					case MyFirstPluginParams::kParamEq2g:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							eqg[1] = gRange->toPlain(value);
							g[1] = true;
						}
					break;

					//Band 3
					case MyFirstPluginParams::kParamEq3o:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							freqOffset[2] = oRange2->toPlain(value);
							f0[2] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq3q:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							//eqq[2] = qRange->toPlain(value);
							eqq[2] = qLogscale->scale(value);
							q[2] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq3g:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							eqg[2] = gRange->toPlain(value);
							g[2] = true;
						}
						break;

						//Band 4
					case MyFirstPluginParams::kParamEq4o:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							freqOffset[3] = oRange3->toPlain(value);
							f0[3] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq4q:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							//eqq[3] = qRange->toPlain(value);
							eqq[3] = qLogscale->scale(value);
							q[3] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq4g:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							eqg[3] = gRange->toPlain(value);
							g[3] = true;
						}
						break;

						//Band 5
					case MyFirstPluginParams::kParamEq5o:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							freqOffset[4] = oRange4->toPlain(value);
							f0[4] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq5q:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							//eqq[4] = qRange->toPlain(value);
							eqq[4] = qLogscale->scale(value);
							q[4] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq5g:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							eqg[4] = gRange->toPlain(value);
							g[4] = true;
						}
						break;

						//Band 6
					case MyFirstPluginParams::kParamEq6o:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							freqOffset[5] = oRange5->toPlain(value);
							f0[5] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq6q:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							//eqq[5] = qRange->toPlain(value);
							eqq[5] = qLogscale->scale(value);
							q[5] = true;
						}
						break;
					case MyFirstPluginParams::kParamEq6g:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
							kResultTrue) {
							eqg[5] = gRange->toPlain(value);
							g[5] = true;
						}
						break;
				}
			}
		PlugProcessor::calc_params(f0,g,q);
		}
	}
	

	//--- Process Audio---------------------
	//--- ----------------------------------
	if (data.numInputs == 0 || data.numOutputs == 0)
	{
		// nothing to do
		return kResultOk;
	}

	if (data.numSamples > 0)
	{
		int32 numChannels = data.inputs[0].numChannels;
		
		//Get the input and output buffers
		uint32 sampleFramesSize = getSampleFramesSizeInBytes(processSetup, data.numSamples);
		void** in = getChannelBuffersPointer(processSetup, data.inputs[0]);
		void** out = getChannelBuffersPointer(processSetup, data.outputs[0]);

		//---check if silence---------------
		// normally we have to check each channel (simplification)
		if (data.inputs[0].silenceFlags != 0)
		{
			// mark output silence too
			data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

			// the Plug-in has to be sure that if it sets the flags silence that the output buffer are
			// clear
			for (int32 i = 0; i < numChannels; i++)
			{
				// do not need to be cleared if the buffers are the same (in this case input buffer are
				// already cleared by the host)
				if (in[i] != out[i])
				{
					memset(out[i], 0, sampleFramesSize);
				}
			}

			// nothing to do at this point
			return kResultOk;
		}

		// mark our outputs is not silent
		data.outputs[0].silenceFlags = 0;	
		
		if (data.symbolicSampleSize == Vst::kSample32) {
			
			
			
			for (int32 i = 0; i < numChannels; i++)
			{
				int32 samples = data.numSamples;
				int32 samplesOrig = samples;
				//Vst::Sample32* ptrIn = (Vst::Sample32*)in[i];
				float* ptrIn = (float*)in[i];
				//Vst::Sample32* ptrOut = (Vst::Sample32*)out[i];
				float* ptrOut = (float*)out[i];
				float* ptrInOri = ptrIn;
				float* ptrOutOri = ptrOut;

				
				for (int k = 0; k < 6; k++) {
					//if (eqg[k] == 0) {
					//	memcpy(out[i], in[i], sampleFramesSize);
					//}
					//else 
					{
						while (--samples >= 0){
							(*ptrOut) =(b0[k] / a0[k]) * (*ptrIn)
									+(b1[k] / a0[k])*previousInputSample32[k][i][0]
									+(b2[k] / a0[k])*previousInputSample32[k][i][1]
									-(a1[k] / a0[k])*previousSamples32[k][i][0]
									-(a2[k] / a0[k])*previousSamples32[k][i][1];

												
							previousInputSample32[k][i][1] = previousInputSample32[k][i][0];
							previousInputSample32[k][i][0] = (*ptrIn);
							previousSamples32[k][i][1] = previousSamples32[k][i][0];
							previousSamples32[k][i][0] = (*ptrOut);
							(*ptrIn) = (*ptrOut);
							ptrIn++;
							ptrOut++;
						}
					}
					samples = samplesOrig;
					ptrIn = ptrInOri;
					ptrOut = ptrOutOri;
					//memcpy(ptrIn, ptrOut, sampleFramesSize);
				}
			}
			
		}else {
			/*
			for (int32 i = 0; i < numChannels; i++)
			{
				// do not need to be copied if the buffers are the same
				if (in[i] != out[i])
				{
					memcpy(out[i], in[i], sampleFramesSize);
				}
			}
			
			for (int32 i = 0; i < numChannels; i++)
			{
				int32 samples = data.numSamples;
				Vst::Sample64* ptrIn = (Vst::Sample64*)in[i];
				Vst::Sample64* ptrOut = (Vst::Sample64*)out[i];

				while (--samples >= 0)
				{
					(*ptrOut) = (Vst::Sample64)((b0 / a0) * (*ptrIn)
						+ (b1 / a0) * previousInputSample32[0]
						+ (b2 / a0) * previousInputSample32[1]
						- (a1 / a0) * previousSamples32[0]
						- (a2 / a0) * previousSamples32[1]);
					previousInputSample32[1] = previousInputSample32[0];
					previousInputSample32[0] = (*ptrIn);
					previousSamples32[1] = previousSamples32[0];
					previousSamples32[0] = (*ptrOut);
					ptrIn++;
					ptrOut++;
				}
			}
			*/
			
		
		}
		
		
	}
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setState (IBStream* state)
{
	if (!state)
		return kResultFalse;

	// called when we load a preset or project, the model has to be reloaded

	IBStreamer streamer (state, kLittleEndian);

	float savedParam1 = 0.f;
	if (streamer.readFloat (savedParam1) == false)
		return kResultFalse;

	int32 savedParam2 = 0;
	if (streamer.readInt32 (savedParam2) == false)
		return kResultFalse;

	int32 savedBypass = 0;
	if (streamer.readInt32 (savedBypass) == false)
		return kResultFalse;

	mParam1 = savedParam1;
	mParam2 = savedParam2 > 0 ? 1 : 0;
	mBypass = savedBypass > 0;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::getState (IBStream* state)
{
	// here we need to save the model (preset or project)

	float toSaveParam1 = mParam1;
	int32 toSaveParam2 = mParam2;
	int32 toSaveBypass = mBypass ? 1 : 0;

	IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat (toSaveParam1);
	streamer.writeInt32 (toSaveParam2);
	streamer.writeInt32 (toSaveBypass);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
