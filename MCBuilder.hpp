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

// Multiple inclusion guards
#ifndef MCBUILDER_HPP
#define MCBUILDER_HPP

#include "Pricer.hpp"
#include "MIS.hpp"

#include <thread>

// Convenience alias for the thread vector and the multiple output list
// Basically they are used to gather multiple pricing output for later use (Output class)
using MultiOutputList	= std::vector< std::tuple<PricerResults, Statistics> >;

// Builder class that puts everything together
template <class ISDE, class IRNG, class IPayoff, class IInput, template <class ISDE, class IRNG, class IPayoff, class IInput> class IPricer,
	class IMIS, class IOutput>
	class Builder : public IPricer<ISDE, IRNG, IPayoff, IInput>, private IMIS, private IOutput {

	private:
		// Private members to be used as temporary objects for multipricing
		// Not goint to be used in single option pricing
		MultiOutputList multi_output_list;

	public:

		// Constructor
		explicit Builder();

		// User informative message
		void message();

		// Mediator method run() that provides a basic user-interaction interface and calls the system components accordingly
		// determining the pricing process from beggining to ending
		inline void run() {

			// Multipricing choice
			std::cout << "How many option prices do you want to approximate?\n\n";
			std::cout << "Your answer: "; unsigned int number_of_threads; 
			std::cin >> number_of_threads;
			
			// Check the input and prevent potential input-caused crashes
			while (std::cin.fail()) {
				
				// Reset failbit
				std::cin.clear(); 

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// Try again
				std::cout << "\n\nInvalid value. Try again!\n\n";
				std::cout << "How many option prices do you want to approximate?\n\n";
				std::cout << "Your answer: "; 
				std::cin >> number_of_threads;
			}

			// In case of single option pricing
			if (number_of_threads == 1)	{

				// Assemble data
				IPricer<ISDE, IRNG, IPayoff, IInput>::get();

				std::cout << "\nRunning simulation...\n\n";

				// Start measuring time
				IMIS::StartStopWatch();

				// Begin the pricing process				
				IPricer<ISDE, IRNG, IPayoff, IInput>::GeneralPricer();

				// Stop measuring time
				IMIS::EndStopWatch();

				// Get the output tuples from the pricing process
				auto output_tuple = IPricer<ISDE, IRNG, IPayoff, IInput>::output();
				auto mis_out = IPricer<ISDE, IRNG, IPayoff, IInput>::MIS_output();
				
				// Use the MIS output to compute statistics and make a decision
				IMIS::ComputeStatistics(mis_out);
				IMIS::ExactPrice(mis_out);
				IMIS::DecisionMaking(mis_out);

				// Extract the MIS output with the computed statistics
				auto mis_output = IMIS::getStatistics();

				// Choose Output Format and provide the MIS and Pricer output to be used in that format
				IOutput::Print(output_tuple, mis_output); 

				// Inform the user that the printing is over
				IOutput::done();	
			}

			// In case of multiple option pricing 
			else if (number_of_threads > 1) {

				std::cout << "\nThe option data, and will remain constant!\n";

				// Assemble data
				IPricer<ISDE, IRNG, IPayoff, IInput>::get();

				// For as many as selected simulations...
				for (unsigned i = 0; i < number_of_threads; i++) {

					// Define a lambda that process the pricing request and saves the outcome of each simulation 
					// in a multi output list for later use
					auto multiPricer = [&]() {
				
						// Start measuring time
						IMIS::StartStopWatch();

						// Begin the pricing process
						IPricer<ISDE, IRNG, IPayoff, IInput>::GeneralPricer();

						// Stop measuring time
						IMIS::EndStopWatch();

						std::cout << "\nSimulation complete!\n\n";

						// Get the output tuples from the pricing process
						auto general_output = IPricer<ISDE, IRNG, IPayoff, IInput>::output();
						auto mis_out = IPricer<ISDE, IRNG, IPayoff, IInput>::MIS_output();
							
						// Use the MIS output to compute statistics and make a decision
						IMIS::ComputeStatistics(mis_out);
						IMIS::ExactPrice(mis_out);
						IMIS::DecisionMaking(mis_out);

						// Get the computed statistics
						auto mis_output = IMIS::getStatistics();

						// Reset the stopwatch for the next iteration
						IMIS::ResetStopWatch();

						// Store the output of this simulation
						multi_output_list.push_back(std::make_tuple(general_output, mis_output));

						// Clear the vectors for the next iteration
						IPricer<ISDE, IRNG, IPayoff, IInput>::ClearStockFlunctuationsVector();
						IPricer<ISDE, IRNG, IPayoff, IInput>::ClearTempOptionPriceVector();
					};
	
					// Run the pricing process
					std::cout << "\nRunning simulation...\n\n";
					std::thread(multiPricer).join();
						
					// For n-1 times ask the user to determine the next option to be priced
					if (i < (number_of_threads - 1)) {

						// Update the option type for the next round
						auto payoff_tuple = IPricer<ISDE, IRNG, IPayoff, IInput>::payoff();

						// Update the Pricer class parameters that are going to be used in the next iteration
						IPricer<ISDE, IRNG, IPayoff, IInput>::setPayoffParameter(std::get<0>(payoff_tuple), std::get<1>(payoff_tuple));
					}
				}
	
				// Now all options has been pricing and we are going to use the multi-output list for printing
				// Choose Output Format by passing the list of multiple outputs to be used iteratively inside the multi-print function
				IOutput::MultiPrint(multi_output_list); 

				// Inform the user that the printing is over
				IOutput::done();

			}
			else {
				// In case of wrong input, print an error message
				std::cout << "\n\nNon-Deterministic Request\n";
			}
		}
	
		// Indicate the end of the program
		void bye();

		// Destructor
		~Builder();
};

#ifndef MCBUILDER_CPP
#include "MCBuilder.cpp"
#endif // !MCBUILDER_CPP

#endif // !MCBUILDER_HPP