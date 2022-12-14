declare filename "sine.dsp"; declare name "sine"; declare version "2.50.2";
declare compile_options "-single -scal -I libraries/ -I project/ -lang wasm";
declare library_path0 "/libraries/stdfaust.lib";
declare library_path1 "/libraries/maths.lib";
declare library_path2 "/libraries/platform.lib";
declare filename "FaustDSP";
declare maths_lib_author "GRAME";
declare maths_lib_copyright "GRAME";
declare maths_lib_license "LGPL with exception";
declare maths_lib_name "Faust Math Library";
declare maths_lib_version "2.5";
declare name "FaustDSP";
declare platform_lib_name "Generic Platform Library";
declare platform_lib_version "0.2";
ID_0 = fconstant(int fSamplingFreq, <math.h>);
ID_1 = 1.0f, ID_0;
ID_2 = (ID_1 : max);
ID_3 = 1.92e+05f, ID_2;
ID_4 = (ID_3 : min);
ID_5 = 1000, ID_4;
ID_6 = (ID_5 : /);
ID_7 = _, ID_6;
ID_8 = (ID_7 : +);
ID_9 = ID_8 ~ \(x1).(x1,(x1 : floor) : -);
ID_10 = ID_9, 2;
ID_11 = (ID_10 : *);
ID_12 = ID_11, 3.1415927f;
ID_13 = ID_12 : *;
ID_14 = (ID_13 : sin);
ID_15 = ID_14, 0.75f;
ID_16 = ID_15 : *;
process = ID_16;
