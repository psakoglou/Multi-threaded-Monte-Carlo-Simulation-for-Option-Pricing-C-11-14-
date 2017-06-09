/*
*	Advanced C++11/14 with Multidisciplinary Applications
*
*	Pavlos Sakoglou
*
*  ===================================================================================================================================================
*
*	Monte Carlo Option Pricing Application - Plain procedural MC simulation for Option Pricing
*
*/

#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>

// Boost Libraries Dependency!!!
// Install Boost before you run
// http://www.boost.org/
#include <boost/random/normal_distribution.hpp>
#include <boost/math/distributions.hpp> 

int main() {

	// Testing plain Monte Carlo 
	// All of the model parameters and input is hard coded here -- trade-off between speed and flexibility
	// The pricing algorithm is hard-coded below. No containers are used for temporary value storage and thus makes the following code
	// appropriate for large simulations without the fear of potential crash.

	std::cout << "**********************************************************************\n";
	std::cout << "*\n* Final Project for QuantNet's Advanced C++11/C++14 Certificate \n*\n";
	std::cout << "* Monte Carlo Option Pricing \n*\n";
	std::cout << "* Pavlos Sakoglou \n*\n";
	std::cout << "**********************************************************************\n\n";

	std::cout << "Plain Monte Carlo Test: Euro Put\n\n";

	// Get the option data values
	double vol				= 0.3;		// Volatility
	double r				= 0.08;		// Rate
	double T				= 0.25;		// Expiry
	double S				= 60;		// Stock price
	double K				= 65;		// Strike price

	unsigned long NSteps, NSIM;

	std::cout << "Explicit Euler Approximation\n\n";
	std::cout << "Number of Simulations: "; std::cin >> NSIM;
	std::cout << "Number of steps: "; std::cin >> NSteps;

	// Model dummy parameters 
	double VOld, VNew;
	double price = 0;
	double m_price = 0;

	// For time sub-intervals and the Weiner process
	double dt = T / static_cast<double>(NSteps);
	double dt_sq = sqrt(dt);

	// Generate random variates of N(0,1)
	std::normal_distribution<double> nor(0, 1);
	std::mt19937 eng;
	double dW;

	// Start measuring time
	auto start_time = std::chrono::system_clock::now();

	// Simulation begins
	for (unsigned i = 1; i <= NSIM; ++i) {

		// Update to the initial stock price
		VOld = S;

		if ((i / 10000) * 10000 == i) {
			// Give status after each 1000th iteration

			std::cout << i << std::endl;
		}
			
		// Discretize the time into N steps
		for (unsigned j = 0; j <= NSteps; j++) {

			dW = nor(eng);

			// Update in every step according to the Euler method
			VNew = VOld + dt * r * VOld +(dt_sq * vol * VOld * dW);

			// Update the price 
			VOld = VNew;
		}

		// Update the average payoff sum
		price += std::max(VNew - K, 0.0) / NSIM;
	}

	// Discount and average the price
	m_price = (price) * exp(-r * T);

	// Stop measuring time
	auto end_time = std::chrono::system_clock::now();

	// Implement the Black-Schole formula for Call 
	auto exact_price = [&]() {
		boost::math::normal_distribution<double> n(0, 1);
		double d1 = std::move((log(S / K) + (r + pow(vol, 2) / 2)*T) / (vol*sqrt(T)));
		double d2 = std::move(d1 - vol*sqrt(T));
		return S*exp((r - r)*T)*cdf(n, d1) - K*exp(-r*T)*cdf(n, d2);
	};

	// Print the approximated and the exact price 
	std::cout << "\nApproximation price: " << m_price << "\n";
	std::cout << "Exact Price: " << exact_price() << "\n\n";

	// Print the rest of the parameters
	std::cout << "*** Simulation input parameters ***\n\n";
	std::cout << "Rate of Return: "		<< r << "\n";
	std::cout << "Strike Price: "		<< K << "\n";
	std::cout << "Expiry Time: "		<< T << "\n";
	std::cout << "Stock Price: "		<< S << "\n";
	std::cout << "Volatility: "			<< vol << "\n";
	std::cout << "NSIM: "				<< NSIM << "\n";
	std::cout << "NSteps: "				<< NSteps << "\n\n";

	std::cout << "*** Simulation model parameters ***\n\n";
	std::cout << "Explicit Euler Approximation\n";
	std::cout << "Mersenne Twister Random Engine\n\n";
	std::cout << "Euro Put\n";
	
	std::cout << "Elapsed time: " << std::chrono::duration<double>(end_time - start_time).count() << "\n\n";

	// In case you are running the .exe file without using VS
	system("pause");

	return 0;
}
