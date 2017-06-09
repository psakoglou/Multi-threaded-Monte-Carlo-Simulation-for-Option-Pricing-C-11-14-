/*
*	Advanced C++11/14 with Multidisciplinary Applications
*
*	Pavlos Sakoglou
*
*  ===================================================================================================================================================
*
*	Monte Carlo Option Pricing Application - User Input class
*
*/

// Multiple inclusion guards
#ifndef INPUT_HPP
#define INPUT_HPP

#include <tuple>
#include <iostream>

// Alias for the tuple that holds the option parameters:
// volatility, interest rate, expiry time, stock price, strike price, number of simulations
using OptionData = std::tuple<double, double, double, double, double, unsigned long>;

// Input class that is designed to get and set the necessary input parameters from the user
class Input {
public:

	// Core members for the option parameters
	double vol;				// Volatility (sigma)
	double r;				// Interest rate
	double T;				// Expiry time (in years, i.e. T = 1 -> derivative expires in 1 year)
	double S;				// Stock Price (spot price)
	double K;				// Strike Price
	unsigned long NSIM;		// Number of simulations

public:
	
	// Constructors 
	explicit Input();
	explicit Input(const Input & inp);

	// Parameter constructors 
	explicit Input(const OptionData & op);
	explicit Input(double & vol_, double & r_, double & T_, double & S_, double & K_, unsigned long & NSIM_);
	explicit Input(double && vol_, double && r_, double && T_, double && S_, double && K_, unsigned long && NSIM_);

	// Assignment operator
	Input & operator=(const Input & inp);

	// Convenience setter
	void setParameters(double vol_, double r_, double T_, double S_, double K_, unsigned long NSIM_);
		
	// Convenience Getter
	const OptionData getOptionData() const;

	// User-interactive interface to set all member data
	inline OptionData setOptionData() {

		std::cout << "\nInput the parameters of the option: \n";

		// Get the parameter values from the user and set the data members to them
		try {
			std::cout << "Volatility: ";				std::cin >> vol;

			// Check input and prevent potential crashes
			if (vol > 10 || vol < 0 || std::cin.fail()) {

				// Reset failbit
				std::cin.clear();

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// Wrong input. Set volatility to 0.1
				std::cout << "\nWrong input for volatility: Default setting: vol = 0.1 \n";
				vol = 0.1;
			}

			std::cout << "Interest Rate: ";				std::cin >> r;

			// Check input and prevent potential crashes
			if (r > 10 || r < 0 || std::cin.fail()) {

				// Reset failbit
				std::cin.clear();

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// Wrong input. Set interest rate to 0.1
				std::cout << "\nWrong input for interest rate: Default setting: r = 0.1 \n";
				r = 0.1;
			}
			std::cout << "Expiry Time: ";				std::cin >> T;

			// Check input and prevent potential crashes
			if (T < 0 || std::cin.fail()) {

				// Reset failbit
				std::cin.clear();

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// Wrong input. Set expiry time to 0.25
				std::cout << "\nWrong input for Expiry time: Default setting: T = 0.25 \n";
				T = 0.25;
			}

			std::cout << "Stock Price: ";				std::cin >> S;

			// Check input and prevent potential crashes
			if (S < 0 || std::cin.fail()) {

				// Reset failbit
				std::cin.clear();

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// Wrong input. Set Stock price to 100
				std::cout << "\nWrong input for Stock Price: Default setting: S = 100 \n";
				S = 100;
			}

			std::cout << "Strike Price: ";				std::cin >> K;

			// Check for wrong input and potential crashes
			if (K < 0 || std::cin.fail()) {

				// Reset failbit
				std::cin.clear();

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// Wrong input. Set strike price to 120
				std::cout << "\nWrong input for Strike Price: Default setting: K = 120 \n";
				K = 120;
			}

			std::cout << "Number of Simulations: ";		std::cin >> NSIM;

			// Check for wrong input and prevent potential crashes
			if (NSIM < 0 || std::cin.fail()) {

				// Reset failbit
				std::cin.clear();

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// Wrong input. Set NSIM to 100'000
				std::cout << "\nWrong input for Expiry time: Default setting: NSIM = 100.000 \n";
				NSIM = 100'000;
			}
			std::cout << "\n\n";
		}
		catch (std::exception & e) {
			std::cout << e.what() << "\n";
		}

		// Return a tuple with the input values
		return std::move(std::make_tuple(vol, r, T, S, K, NSIM));
	}
	
	// Destructor
	~Input();
};

#endif // !INPUT_HPP
