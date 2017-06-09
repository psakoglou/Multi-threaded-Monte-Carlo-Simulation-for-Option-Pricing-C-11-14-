/*
*	Advanced C++11 / 14 with Multidisciplinary Applications
*
*	Level 7 - Final Project
*
*	Pavlos Sakoglou
*
*  ===================================================================================================================================================
*
*	Monte Carlo Option Pricing Application
*
*/

// Include the main system 
#include "MCBuilder.hpp"
#include "Output.hpp"

int main() {

	// Test Monte Carlo Option Pricing using the Builder class and its component parameters

	std::cout << "**********************************************************************\n";
	std::cout << "*\n* Final Project for QuantNet's Advanced C++11/C++14 Certificate \n*\n";
	std::cout << "* Monte Carlo Option Pricing \n*\n";
	std::cout << "* Pavlos Sakoglou \n*\n";
	std::cout << "**********************************************************************\n\n";
	
	// Instantiate Builder using the appropriate classes as parameters
	Builder<FDM_SDE, RNG, Payoff, Input, Pricer, MIS, Output> builder;
	builder.message();		// Print a message for the user
	builder.run();			// Let the user determine the MC parameters and input, price the derivative, and print the results
	builder.bye();			// Indicate that the process is over and the program ends

	// In case you are running the .exe file without using VS
	system("pause");

	return 0;
}