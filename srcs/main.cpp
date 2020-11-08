
// INCLUDES

#include "../includes/hello_world.h"
#include "../includes/model_system.h"
#include <iostream>


// Main function 

// Main function is used for test of the function  model_system
int main(void){


	//VARIABLES FOR TEST
	float Ri = 1e3; // resistance of the inspiration valvle in Pa.m-1.s
	float Re = 1e3; // resistance of the expiration valve in Pa.m-1.s
	float Rf = 5e6;  // resistance of the leak Pa.m-1.s, almost no leak
	float R = 5e6; // resistance of the patient in Pa.m-1.s
	float C = 100e-9; // compliance of the patient in m3.Pa-1
	float Pbl = 30e3; // relative output pressure of the blower in Pa
	float Patm = 0; // Pa
	float Vp = 0.1e-3; // patient's lungs volume in m3
	float t = 0; // time in s
	float dt = 0.1; // integration time in s
	float T = 5; // period of cycle in s
	float alpha = 0.5; //proportion of inspiration

	std::cout << "\nsimulation startup\n";

	for(int i = 0; i<100; i++){ // 100 time step

		t += dt; // new time step

		resistance_valves(t, T, alpha, &Ri, &Re); //computation of the resistance of the valves

		Vp = model_system(Ri, Re, Rf, R, C, Pbl, Patm, Vp, dt); // computation of the new vomule in the lungs

		std::cout << Vp * 1e6 << " ml\n";
	}

	std::cout << "\nsimulation finished\n";
	
	return 0;
}