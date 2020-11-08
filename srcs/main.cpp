
// INCLUDES

#include "../includes/hello_world.h"
#include "../includes/model_system.h"
#include <iostream>


// Main function 

int main(void){


	//VARIABLES FOR TEST
	float Ri = 500e6;
	float Re = 500e6;
	float Rf = 5e3; //almost no leak
	float R = 5e6; // Pa.m-1.s
	float C = 100e-9; // m3.Pa-1
	float Pbl = 30e3; // Pa
	float Patm = 0; // Pa
	float Vp = 0.1e-3; // m3
	float t = 0;
	float dt = 0.1;
	float T = 5; //s
	float alpha = 0.5; //proportion of inspiration

	std::cout << "\nsimulation startup\n";

	for(int i = 0; i<100; i++){

		t += dt; // new time step

		resistance_valves(t, T, alpha, &Ri, &Re); //computation of the resistance of the valves

		Vp = model_system(Ri, Re, Rf, R, C, Pbl, Patm, Vp, dt); // computation of the new vomule in the lungs

		std::cout << Vp * 1e6 << "\n";
	}

	std::cout << "\n simulation over";
	
	return 0;
}