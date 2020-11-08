

#ifndef H_MODEL_SYSTEM
#define H_MODEL_SYSTEM

float model_system(float Ri, float Re, float Rf, float R, float C, float Pbl, float Patm, float Vp, float dt);
void resistance_valves(float t, float T, float alpha, float * pt_Ri, float * pt_Re);

#endif