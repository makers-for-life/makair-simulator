//INCLUDES
#include "../includes/model_system.h"
#include <tgmath.h>

//FUNCTION
float model_system(float Ri, float Re, float Rf, float R, float C, float Pbl, float Patm, float Vp, float dt)
{
    // resistance of expiration valve and leak are in paralell
    float Ref = Re * Rf / (Re + Rf); 

    // influence of blower and atm pressure on the flow (derivative of the volume)
    float P_factor = (Ri * Patm + Ref * Pbl) / (R * (Ref + Ri) + Ref * Ri);

    // influence of vomule present in patient's lungs on the flow
    float V_factor = -Vp * (Ref + Ri) / (C * (R * (Ref + Ri) + Ref * Ri));

    float flow = P_factor + V_factor;

    // conputation of the new wolume
    return Vp + flow * dt;

}

void resistance_valves(float t, float T, float alpha, float * pt_Ri, float * pt_Re){

    //Inspiration phase : the inspiration vavle is open, expiration valve is closed
    if(fmod(t, T) <= alpha * T){
        *pt_Re = 5e6;
        *pt_Ri = 0;
    }
    
    //expiration phase : the expiration valve is open, inspiration valve is closed
    else{
        *pt_Re = 0;
        *pt_Ri = 5e6;
    }
}