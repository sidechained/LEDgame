/* ------------------------------------------------------------
name: "sine", "FaustDSP"
version: "2.50.2"
Code generated with Faust 2.49.1 (https://faust.grame.fr)
Compilation options: -lang cpp -es 1 -mcd 16 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

// minimal DSP class
class dsp {
	protected:
		int fSamplingFreq;
};

// For mydsp class:

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif


class mydsp : public dsp {
	
 private:
	
	int fSampleRate;
	float fConst0;
	float fRec0[2];
	
 public:
	
	// void metadata(Meta* m) { 
	// 	m->declare("compile_options", "-single -scal -I libraries/ -I project/ -lang wasm");
	// 	m->declare("filename", "sine.dsp");
	// 	m->declare("library_path0", "/libraries/stdfaust.lib");
	// 	m->declare("library_path1", "/libraries/maths.lib");
	// 	m->declare("library_path2", "/libraries/platform.lib");
	// 	m->declare("maths_lib_author", "GRAME");
	// 	m->declare("maths_lib_copyright", "GRAME");
	// 	m->declare("maths_lib_license", "LGPL with exception");
	// 	m->declare("maths_lib_name", "Faust Math Library");
	// 	m->declare("maths_lib_version", "2.5");
	// 	m->declare("name", "sine");
	// 	m->declare("platform_lib_name", "Generic Platform Library");
	// 	m->declare("platform_lib_version", "0.2");
	// 	m->declare("version", "2.50.2");
	// }

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 1;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = 1e+03f / std::min<float>(1.92e+05f, std::max<float>(1.0f, float(fSampleRate)));
	}
	
	virtual void instanceResetUserInterface() {
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			fRec0[l0] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	// virtual void buildUserInterface(UI* ui_interface) {
	// 	ui_interface->openVerticalBox("sine");
	// 	ui_interface->closeBox();
	// }
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			fRec0[0] = fConst0 + (fRec0[1] - std::floor(fRec0[1]));
			output0[i0] = FAUSTFLOAT(0.75f * std::sin(6.2831855f * fRec0[0]));
			fRec0[1] = fRec0[0];
		}
	}

};

#endif
