/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ===================================================================================================================================================
*
*	Monte Carlo Option Pricing Application - Finite Differences / Stochastic Diff. Equations class
*
*/

// Multiple inclusion guards
#ifndef FDM_SDE_HPP
#define FDM_SDE_HPP

#include <string>
#include <iostream>
#include <tuple>

// Stochastic Differential Equations and Finite Differences Methods class that models SDEs and FDM models
class FDM_SDE {
private:
	// Class members that hold the selected FDM method approach and its name
	int fdm_choice;			// Hold the FDM method choice
	std::string fdm_name;	// Hold its name for MIS purposes
public:

	// Constructors 
	explicit FDM_SDE();
	explicit FDM_SDE(const FDM_SDE & fdm);
	explicit FDM_SDE(const int choice, const std::string fdm_name_);

	// Assignment Operator
	FDM_SDE & operator=(const FDM_SDE & fdm);

	// Setter
	void setFDM(const int choice, const std::string & fdm_name_);

	// Getters
	const int getFDMchoice() const;
	const std::string getFDMname() const;

	// SDE models

	// 1. For Geometric Brownian Motion approach
	inline static double GBM(double & S, double & T, double & sigma, double & r) {
		return S * exp(T*(r - 0.5*sigma*sigma));
	}

	// 2. For Explicit Euler approach

	// Drift function (mu)
	inline static double drift(double & r, double & S) {
		return r*S;
	}

	// Diffusion function (sigma)
	inline static double diffusion(double & vol, double & S) {
		double betaCEV = 1.0;
		return vol * std::pow(S, betaCEV);
	}

	// 3. For Milstein approach

	// Diffusion derivative function (sigma prime)
	inline static double diffusion_derivative(double & vol, double & S) {
		double betaCEV = 1.0;
		return 0.5 * vol * (betaCEV)* pow(S, 2.0 * betaCEV - 1.0);
	}

	// 4. Add another method's supportive functions below to extend the functionality, i.e. Centered/Forward Approximations, etc.
	// Don't forget to modify FDM() below so that the user can choose it for pricing
	// Lastly, add an extra conditional statement and the algorithm in Pricer<...> class
	// See 'readme' file for more details

	// User-interactive interface to determine the FDM model to be used
	inline std::tuple<int, std::string> FDM() {

		// Exception guard
		try {
			// Appropriate user messages to choose a model
			std::cout << "\n\nWhat kind of FDM method you want to use in the evaluation?\n";
			std::cout << "1. Geometric Brownian Motion\n";
			std::cout << "2. Explicit Euler Method\n";
			std::cout << "3. Milstein Method\n\n";
			// In case you add more FDM models, add another choice here, and adapt the code below likewise

			// Get the user's choice of the model
			std::cout << "Your answer: "; std::cin >> fdm_choice;

			// Check the input and prevent potential input-caused crashes
			while (std::cin.fail()) {

				// Reset failbit
				std::cin.clear();

				// User didn't input a number
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				std::cout << "\n\nInvalid value. Try again!\n\n";

				// Appropriate user messages to choose a model
				std::cout << "What kind of FDM method you want to use in the evaluation?\n";
				std::cout << "1. Geometric Brownian Motion\n";
				std::cout << "2. Explicit Euler Method\n";
				std::cout << "3. Milstein Method\n\n";

				// Get the user's choice of the model
				std::cout << "Your answer: "; 	std::cin >> fdm_choice;
			}

			// Check user's choice
			switch (fdm_choice) {
			case 1:
				// GBM selected, set appropriately
				std::cout << "Choice of Finite Differences Approximation: No FDM. Using GBM Model\n";
				fdm_name = "GBM";
				break;


			case 2:
				// Explicit Euler selected, set appropriately
				std::cout << "Choice of Finite Differences Approximation: Explicit Euler\n\n";
				fdm_name = "Explicit Euler";
				break;


			case 3:
				// Milstein method selected, set appropriately
				std::cout << "Choice of Finite Differences Approximation: Milstein method\n\n";
				fdm_name = "Milstein Method";
				break;


			default:
				// Wrong input. Set to GBM model
				std::cout << "Invalid choice. Using GBM Model\n";
				fdm_name = "GBM";
				fdm_choice = 1;
				break;
			}
		}
		// Handle an exception error
		catch (std::exception & e) {
			std::cout << e.what() << "\n\n";
		}

		// Return the tuple of choices
		return std::make_tuple(fdm_choice, fdm_name);
	}

	// Constructor
	~FDM_SDE();
};

#endif // !FDM_SDE_HPP
