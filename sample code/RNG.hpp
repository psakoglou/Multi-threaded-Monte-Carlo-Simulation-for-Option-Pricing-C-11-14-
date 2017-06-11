/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ===================================================================================================================================================
*
*	Monte Carlo Option Pricing Application - Random Number Generator class
*
*/

// Multiple inclusion guards
#ifndef RNG_HPP
#define RNG_HPP

// Necessary files to for std::function and std::string
#include <functional>
#include <string>
#include <random>
#include <iostream>

// std::chrono will be used to retain randomness -- we use current time as a seed
#include <chrono>

// Alias for the function wrapper that holds the selected random generating function
using RNGFunctionType = std::function<double(void)>;

// Random Number Generation class
class RNG {
private:
	// Private members that hold the user's decision on what engine to use in each simulation
	RNGFunctionType m_random;	// Function wrapper to hold the random generating function given a selected engine
	std::string engine_name;	// String variable to hold the name of the selected engine, for MIS purposes
public:

	// Constructors 
	explicit RNG();
	explicit RNG(const RNG & rng);
	explicit RNG(const RNGFunctionType rng_function, const std::string engine_name_);
	
	// Assignment operator
	RNG & operator=(const RNG & rng);

	// Setter
	void EngineSetter(const RNGFunctionType rng_function_, const std::string engine_name_);

	// Getter
	const RNGFunctionType NormalGenerator() const;
	const std::string EngineName() const;

	// Generate N(0,1) variates using Default Random Engine
	inline static double DefaultRandomEngine() {

		// Using normal_distribution from <random> library or mean = 0 and variance = 1 (Standard Normal)
		// See more here: http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
		std::normal_distribution<double> n(0, 1);

		// In each call of this function the seed will be updated by the current time
		auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

		// Feed the seed to the engine to reboot it
		std::default_random_engine eng((unsigned int)seed);

		// Feed the engine to the distribution and get a random N(0,1) variate
		return n(eng);
	}

	// Generate N(0,1) RV using Mersenne Twister engine
	inline static double MersenneTwisterEngine() {

		// Using normal_distribution from <random> library or mean = 0 and variance = 1 (Standard Normal)
		std::normal_distribution<double> n(0, 1);

		// In each call of this function the seed will be updated by the current time
		auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

		// Feed the seed to the engine to reboot it
		std::mt19937 eng((unsigned int)seed);

		// Feed the engine to the distribution and get a random N(0,1) variate
		return n(eng);
	}

	// Add another engine here
	// Don't forget to modify Gaussian() below so that the user can choose it for random generation
	// See 'readme' file for more details

	// User-friendly interface to determine the engine to be used
	inline std::tuple<RNGFunctionType, std::string> Gaussian() {

		// Exception guard
		try {
			// Appropriate user messages to choose an engine
			std::cout << "Choose Random Generation Engine:\n\n";
			std::cout << "1. Default Random Engine\n";
			std::cout << "2. Mersenne Twister Random Engine\n\n";
			// In case you add more RNG types, add another choice here, and adapt the code below likewise

			// Dummy variables to hold the input
			unsigned choice;

			// Get the user's choice
			std::cout << "Your answer: "; 	std::cin >> choice;

			// Check for wrong input and prevent potential program crash
			while (std::cin.fail()) {

				// Reset failbit
				std::cin.clear();

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				std::cout << "\nInvalid value. Try again!\n\n";
				// Appropriate user messages to choose an engine
				std::cout << "Choose Random Generation Engine:\n\n";
				std::cout << "1. Default Random Engine\n";
				std::cout << "2. Mersenne Twister Random Engine\n";
				std::cout << "3. Box-Muler Algorithm\n\n";

				// Get the user's choice
				std::cout << "Your answer: "; 	std::cin >> choice;
			}

			// Bind the appropriate member random generating function to the function wrapper member
			switch (choice) {


			case 1:
				// Default Engine selected, set appropriately
				std::cout << "\nYou chose: Default Random Engine\n";
				m_random = std::bind(&RNG::DefaultRandomEngine);
				engine_name = "Default Random Engine";
				break;

			case 2:
				// Mersenne Twister selected, set appropriately
				std::cout << "\nYou chose: Mersenne Twister Engine\n";
				m_random = std::bind(&RNG::MersenneTwisterEngine);
				engine_name = "Mersenne Twister";
				break;

			default:
				// Invalid choice
				std::cout << "No valid choice was selected. Set Default Engine\n";
				m_random = std::bind(&RNG::DefaultRandomEngine);
				engine_name = "Default Random Engine";
				break;
			}
		}
		// Handle an exception error
		catch (std::exception & e) {
			std::cout << e.what() << "\n\n";
		}

		// Return the tuple of choices
		return std::make_tuple(m_random, engine_name);
	}

	// Destructor
	~RNG();
};

#endif // !RNG_HPP
