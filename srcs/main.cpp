/******************************************************************************
 * @author Makers For Life
 * @copyright Copyright (c) 2020 Makers For Life
 * @file main.cpp
 * @brief Main function of the makair simulator
 *****************************************************************************/

// INCLUDES


#include "../includes/simulator.h"
#include<iostream>


// Main function 
int main(void){

	Simulator simulator;
	std::cout << "program started";
	simulator.run();
	
	return 0;
}
