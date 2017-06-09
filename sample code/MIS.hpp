/*
*	Advanced C++11 / 14 with Multidisciplinary Applications
*
*	Pavlos Sakoglou
*
*  ===================================================================================================================================================
*
*	Monte Carlo Option Pricing Application - Management Information System class for system's data
*
*/

// Multiple inclusion guards
#ifndef MIS_HPP
#define MIS_HPP

#include <chrono>

#include <boost/random/normal_distribution.hpp>
#include <boost/math/distributions.hpp> 

#include "Pricer.hpp"
#include "RNG.hpp"
#include "Payoff.hpp"
#include "Input.hpp"

// Alias for the MIS output tuple: mean price, max price, min price, SD, SE, and exact price, decision, elapsed time in seconds
using Statistics = std::tuple<double, double, double, double, double, double, bool, double>;

// Alias to improve readability for chrono use
using SystemClock = std::chrono::system_clock;

// MIS class that computes statistics of the system
class MIS {
private:
	// In case user wants to manually provide a pricer output vector and just use this class' functionality
	PricerOutputMIS pricer_results;

	// Member of MIS -- statistics to be computed
	double exact_price;
	double mean_price;
	double max_price;
	double min_price;
	double SD;
	double SE;
	bool decision;
	double elapsed_time;

	// Extras for measuring time with StopWatch
	std::chrono::time_point<SystemClock> start, end;
public:

	// Constructors 
	explicit MIS();
	explicit MIS(const MIS & mis);
	explicit MIS(const PricerOutputMIS & pricer_res);

	// Assignment Operator
	MIS & operator=(const MIS & mis);

	// MIS method to compute statistics given pricer input
	inline void ComputeStatistics(const PricerOutputMIS & pricer_res) {

		// Extract the information from the input tuple
		auto option_data	= std::get<1>(pricer_res);
		auto stock_prices	= std::get<2>(pricer_res);
		auto option_prices	= std::get<3>(pricer_res);

		// Variables to be used in computations below
		double sum = 0.0, sum_sq = 0.0;

		// Extract the necessary option data information
		double r = std::get<1>(option_data);
		double T = std::get<2>(option_data);

		// Get the sums for SE and SD formulas
		std::for_each(std::begin(option_prices), std::end(option_prices), [&](int n) {
			// Sum up the elements
			sum += n;
			sum_sq += pow(n, 2);
		});

		// Get the sum of the stock prices to compute the mean price of the simulations
		mean_price = 0;
		std::for_each(std::begin(stock_prices), std::end(stock_prices), [&](int n) { 
			mean_price += n;
		});

		// Compute the mean stock price
		mean_price /= stock_prices.size();

		// Compute the Standard deviation -- functuations between stock prices
		SD = (sqrt((sum_sq - (1.0 / option_prices.size())*pow(sum, 2)) / (option_prices.size() - 1))) * exp(-r * T);

		// Compute the Standard Error
		SE = SD / sqrt(option_prices.size());

		// Get the max and min price of the stock in the simulation
		max_price = *std::max_element(stock_prices.begin(), stock_prices.end());
		min_price = *std::min_element(stock_prices.begin(), stock_prices.end());

		// One less function call by initializing elapsed time directly
		elapsed_time = std::chrono::duration<double>(end - start).count();
	}
	
	// MIS method that computes the exact prices of the options using the BS formulas
	// This function is of type int to emulate the advantage of switch-case that breaks the 
	// conditional statement flow in case a conditionis satisfied
	inline int ExactPrice(const PricerOutputMIS & pricer_res) {

		// Get the stock price names to discriminate the payoff
		auto names = std::get<4>(pricer_res);

		// Get the option data
		auto option_data = std::get<1>(pricer_res);

		// Get the option data values
		double vol	= std::get<0>(option_data);		// Volatility
		double r	= std::get<1>(option_data);		// Rate
		double T	= std::get<2>(option_data);		// Expiry
		double S	= std::get<3>(option_data);		// Stock price
		double K	= std::get<4>(option_data);		// Strike price

		// Regular expression to be used to indicate if the underlying option is a call or a put
		std::regex reg("(.*)(Call)");

		// If it's Call, use the BS_call formula
		if (std::regex_match(names[2], reg)) {

			boost::math::normal_distribution<double> n(0, 1);

			double d1	= (log(S / K) + (r + pow(vol, 2) / 2)*T) / (vol*sqrt(T));
			double d2	= d1 - vol*sqrt(T);

			exact_price = S*exp((r - r)*T)*cdf(n, d1) - K*exp(-r*T)*cdf(n, d2);

			return 0;
		}
		// If it's Put, use the BS_put formula
		else {

			boost::math::normal_distribution<double> n(0, 1);

			double d1	= (log(S / K) + (r + pow(vol, 2) / 2)*T) / (vol*sqrt(T));
			double d2	= d1 - vol*sqrt(T);

			exact_price = K*exp(-r*T)*cdf(n, -d2) - S*exp((r - r)*T)*cdf(n, -d1);

			return 0;
		}
	}

	// Decision making function that compares the approximated option price to the exact price and determines if they are 'epsilon' close
	// 'epsilon can be determined depending on the accuracy the user wants. Here we pick epsilon = 0.01
	inline void DecisionMaking(const PricerOutputMIS & pricer_res) {

		// If the approximated price is within an open ball with radius of 0.01 from the exact price, then the price is valid
		if (abs(exact_price - std::get<0>(pricer_res)) < 0.01) {
			decision = true;
		}
		// Otherwise reject the price and simulate again
		else {
			decision = false;
		}
	}

	// Stopwatch methods to measure processing time

	// Start measuring time now
	inline void StartStopWatch() {
		start = SystemClock::now();
	}

	// Stop measuring time now
	inline void EndStopWatch() {
		end = SystemClock::now();
	}

	// Reset the timer
	inline void ResetStopWatch() {
		elapsed_time = 0;
	}
	
	// The elapsed time is defined as start time - end time
	inline double ElapsedTime() {
		// Compute the elapsed seconds
		elapsed_time = std::chrono::duration<double>(end - start).count();
		
		// Return
		return elapsed_time;
	}

	// MIS output vector with application statistics
	inline const Statistics getStatistics() const {
		return std::make_tuple(mean_price, max_price, min_price, SD, SE, exact_price, decision, elapsed_time);
	}

	// Destructor 
	~MIS();
};

#endif // !MIS_HPP
