declare filename "noise.dsp"; declare name "noise"; declare version "2.44.5";
declare compile_options "-single -scal -I libraries/ -I project/ -lang wasm";
declare library_path0 "/libraries/stdfaust.lib";
declare library_path1 "/libraries/noises.lib";
declare filename "FaustDSP";
declare name "FaustDSP";
declare noises_lib_name "Faust Noise Generator Library";
declare noises_lib_version "0.4";
ID_0 = _, 12345;
ID_1 = (ID_0 : +);
ID_2 = _, 1103515245;
ID_3 = (ID_2 : *);
ID_4 = ID_1 ~ ID_3;
ID_5 = ID_4, 2147483647.0f;
ID_6 = ID_5 : /;
process = ID_6;
