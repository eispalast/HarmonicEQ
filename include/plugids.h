//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : plugids.h
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

namespace Steinberg {
namespace MyFirstPlugin {

// HERE are defined the parameter Ids which are exported to the host
	enum MyFirstPluginParams : Vst::ParamID
	{
		kBypassId = 100,

		//o= offset, q=q, g=gain

		kParamVolId = 102,
		kParamOnId = 1000,
		kParamEq1f0 = 1001,
		kParamEq1q = 1002,
		kParamEq1g = 1003,
		kParamEq2o = 2001,
		kParamEq2q = 2002,
		kParamEq2g = 2003,
		kParamEq3o = 3001,
		kParamEq3q = 3002,
		kParamEq3g = 3003,
		kParamEq4o = 4001,
		kParamEq4q = 4002,
		kParamEq4g = 4003,
		kParamEq5o = 5001,
		kParamEq5q = 5002,
		kParamEq5g = 5003,
		kParamEq6o = 6001,
		kParamEq6q = 6002,
		kParamEq6g = 6003,

		

		kNumHarmonics= 3000
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID MyProcessorUID (0xD58F09F2, 0xB618454C, 0xA4AB4C3A, 0x809E3CA5);
static const FUID MyControllerUID (0x38BAE76B, 0x205348B1, 0xA39B4DBF, 0xB0B51127);

//------------------------------------------------------------------------
} // namespace MyFirstPlugin
} // namespace Steinberg
