/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  =======================================================================================
*
*	Monte Carlo Option Pricing Application - Option Pricer class
*
*/

// Multiple inclusion guards
#ifndef PRICER_HPP
#define PRICER_HPP

#include <functional>
#include <vector>
#include <string>
#include <tuple>

#include "Payoff.hpp"
#include "FDM_SDE.hpp"
#include "RNG.hpp"

// Alias for Option Data tuple
// i.e Volatility, Rate, Time, Stock, Strike, NSIM, NT (optional)
using OptionData = std::tuple<double, double, double, double, double, unsigned long>;

// Alias for output tuple to be used in MIS class
using PricerOutputMIS = std::tuple<double, OptionData, std::vector<double>, std::vector<double>, std::vector<std::string>>;

 // Alias for the output of Pricer, which is a tuple that holds all the necessary data for MIS and Output classes
using PricerResults = std::tuple< double,						// Option price
								  OptionData,					// OptionData
								  unsigned long,				// NSteps (optionally, otherwise set to 0)
								  std::vector<std::string>,		// Model parameter names
								  double,						// In case of barrier option: upper cap
								  double >;						// In case of barrier option: lower cap
								   									  																	
// Alias for a tuple that holds all the model information
using ModelParameterTuple = std::tuple<RNGFunctionType, int, PayoffFunctionType>;

// Next Generation template Pricer class, that takes the pricing component classes as parameters and uses their functionality
// in a coherent way. In particular, it takes SDE, RNG, Payoff, and Input
template <class ISDE, class IRNG, class IPayoff, class IInput>
class Pricer : public ISDE, public IRNG, public IPayoff, public IInput {
private:

	// Member data that store the input and output parameters of the used model

	// Input
	ModelParameterTuple		 model_parameters;		// To hold the engine-type random generator, the SDE choice, and the Payoff
	std::vector<std::string> parameter_names;		// To hold the names of the pricing model parameters
	OptionData				 option_data;			// To hold the option data (Stock, Strike, etc.)

	// Model Parameters separately to improve flexibility
	PayoffFunctionType	payoff_parameter;
	RNGFunctionType		rng_parameter;
	int					fdm_sde_parameter;

	// Optionally
	bool explicit_euler = false;	// Indicator in case of Explicit Euler approach
	
	// Output
	std::vector<double> stock_flunct;	// To hold the stock flunctuations
	std::vector<double>	option_prices;	// To hold the option prices in each simulation
	double m_price;						// To hold the option price
public:

	// Constructors
	explicit Pricer();
	explicit Pricer(const Pricer & pr);
	explicit Pricer(const ModelParameterTuple & mpt, const std::vector<std::string> & vnames, const OptionData & opt_d);

	// Assignment operator
	Pricer & operator=(const Pricer & pr);

	// In case of Explicit Euler approach
	unsigned long NSteps;		

	// Setters
	void setRNGParameter(const RNGFunctionType rng, const std::string & name);
	void setFDM_SDE(const int fdm, const std::string & name);
	void setParameters(const std::vector<std::string> & name_vec);
	void setModelParameters(const ModelParameterTuple & tup);
	void setOptData(const OptionData & optd);
	void setNSteps(const unsigned long steps);

	// Getters
	const std::vector<std::string>	getParameterNames()		const;
	const ModelParameterTuple		getModelParameters()	const;
	const std::vector<double>		getStockFlunct()		const;
	const std::vector<double>		getOptionPrices()		const;
	const OptionData				getOptionData()			const;
	const double					getPrice()				const;
	const double					getNSteps()				const;

	// Get method that provides an automated user interface for input and model information
	inline void get() {

		// Get option data and set the member data to it
		option_data = IInput::setOptionData();

		// Get the model parameters
		auto RNGtuple		= IRNG::Gaussian();		// Get the random generator
		auto FDMtuple		= ISDE::FDM();			// Get the FDM choice

		// In case of Explicit Euler or Milstein approach
		if (std::get<0>(FDMtuple) != 1) {
			std::cout << "How many steps?\n";
			std::cin >> NSteps;
		}

		// Get the payoff
		auto PAYOFFtuple = IPayoff::payoff();	
	
		// Set the member data for model parameters
		// 1) Random wrapper
		// 2) FDM choice
		// 3) Payoff wrapper 
		model_parameters = std::make_tuple(std::get<0>(RNGtuple), std::get<0>(FDMtuple), std::get<0>(PAYOFFtuple));

		// Initialize the partial model parameters
		rng_parameter			= std::get<0>(model_parameters);
		fdm_sde_parameter		= std::get<1>(model_parameters);
		payoff_parameter		= std::get<2>(model_parameters);

		// Get the names for MIS
		parameter_names.push_back(std::get<1>(RNGtuple));		// Random Generation model
		parameter_names.push_back(std::get<1>(FDMtuple));		// FDM model
		parameter_names.push_back(std::get<1>(PAYOFFtuple));	// Payoff model
	}

	// GeneralPricer() pricing algorithm
	// Determines what type of pricing will be done according to the input parameters of get() or, optionally, hard-coded determined values
	// Works either way since it uses the initialized data memers of Pricer class for safety
	inline double GeneralPricer() {

		// Get the option data values
		double vol			= std::get<0>(option_data);		// Volatility
		double r			= std::get<1>(option_data);		// Rate
		double T			= std::get<2>(option_data);		// Expiry
		double S			= std::get<3>(option_data);		// Stock price
		double K			= std::get<4>(option_data);		// Strike price
		unsigned long NSIM	= std::get<5>(option_data);		// Number of simulations

		// Get the model function wrapper Payoff and FDM
		int fdm_model_choice	= std::get<1>(model_parameters);
		auto option_payoff		= std::get<2>(model_parameters);

		// Model dummy parameters 
		double VOld, VNew;
		double price = 0;

		// In case of Asian options, provide an indicator to enable averaging of stock prices
		double average_price = 0;
		bool asian = false;
		std::regex reg("(Asian)(.*)");
		if (std::regex_match(parameter_names[2], reg)) {
			bool asian = true;
		}
		
		// For time sub-intervals and the Weiner process
		double dt = T / static_cast<double>(NSteps);
		double dt_sq = sqrt(dt);
		
		// Normal Random generation
		std::normal_distribution<double> n(0, 1);
		double Normal;
		
		std::default_random_engine eng;

		if (parameter_names[0] == "Mersenne Twister Engine") eng = std::mt19937(NULL);
		
		// General GBM method
		switch (fdm_model_choice) {

			case 1:	
			{	// General Geometric Brownian Motion
				// Stock update given GBM
				VOld = ISDE::GBM(S, T, vol, r);

				// Simulation begins 
				for (unsigned i = 0; i < NSIM; i++) {

					Normal = n(eng);

					// Simulate a random path
					VNew = VOld * exp(sqrt(vol*vol*T)*Normal);

					// Store the stock flunctuation
					stock_flunct.push_back(VNew);

					if ((i / 10000) * 10000 == i) {
						// Give status after each 1000th iteration
						std::cout << i << std::endl;
					}

					// Update the payoff sum
					double tmp = option_payoff(K, VNew);

					// Get the current option price
					option_prices.push_back(tmp);

					// Update the payoff sum
					price += tmp / static_cast<double>(NSIM);
				}
				// Get the average discounted price of the simulation
				m_price = price * exp(-r*T);
				break;
			}
			case 2: 
			{	// Explicit Euler 
				explicit_euler = true;
	
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

						Normal = n(eng);

						// Update in every step according to the Euler method
						VNew = VOld + dt * ISDE::drift(r, VOld) + (dt_sq * ISDE::diffusion(vol, VOld) * Normal);

						// Update the price
						VOld = VNew;

						if (asian) average_price += VNew;
					}

					// Store the update into the stock flunctuation vector
					stock_flunct.push_back(VNew);

					// Update the payoff sum
					// Use a temporary variable tmp to avoid multiple function calls of payoff()
					double tmp = 0;
					if (asian) tmp = option_payoff(K, average_price/NSteps);
					else tmp = option_payoff(K, VNew);

					// Get the current option price
					option_prices.push_back(tmp);

					// Update the average payoff sum
					price += tmp;
				}

				// Discount and average the price
				m_price = (price / static_cast<double>(NSIM)) * exp(-r * T);
				break;
			}
			case 3:	
			{	// Milstein Method
				
				// Indicator that we are using a model with time discretization
				explicit_euler = true;

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

						Normal = n(eng);

						// Update in every step according to the Milstein method
						VNew = VOld + (dt * ISDE::drift(r, VOld)) + (dt_sq * ISDE::diffusion(vol, VOld) * Normal)
							+ 0.5 * ISDE::diffusion(vol, VOld) * ISDE::diffusion_derivative(vol, VOld) * (pow(dt_sq * Normal, 2) - dt);

						// Update the price
						VOld = VNew;
					
						if (asian) average_price += VNew;
					}

					// Store the update into the stock flunctuation vector
					stock_flunct.push_back(VNew);

					// Update the payoff sum
					double tmp = 0;
					if (asian) tmp = option_payoff(K, average_price / NSteps);
					else tmp = option_payoff(K, VNew);

					// Get the current option price
					option_prices.push_back(tmp);

					// Update the average payoff sum
					price += tmp;
				}

				// Discount and average the price
				m_price = (price / static_cast<double>(NSIM))* exp(-r * T);
				break;
			}
			default:
				// In case of wrong input, print an error message
				std::cout << "Error: No Deterministic Request for Pricing\n";
				m_price = 0;
				break;
		}

		// Finally return the approximated price
		return m_price;
	}

	// Inline setter for Payoff parameters
	// Will be used in the Builder class in case of multi-pricing so that the user can update the options parameters to be prices
	// in real time
	inline void setPayoffParameter(const PayoffFunctionType payoff, const std::string & name) {

		ModelParameterTuple new_model_parameters	= std::make_tuple(std::get<0>(model_parameters), std::get<1>(model_parameters), payoff);
		model_parameters							= new_model_parameters;
		parameter_names[2]							= name;
	}

	// Output tuple for MIS use
	// Returns all the info MIS is going to use
	inline PricerOutputMIS MIS_output() {
		// Pricer outcome that will be used to compute statistics and make trading decisions
		// Tuple to be used from MIS class
		return std::make_tuple(m_price, option_data, stock_flunct, option_prices, parameter_names);
	}

	// Output tuple for Output class use
	// Returns all the info Output class is going to use
	inline PricerResults output() {
		// Check if we used an approximation that required time discetization
		if (explicit_euler) {
			// Return a tuple tha containes all the outcome information of pricer 
			// To be used from Output class
			return std::make_tuple(m_price, option_data, NSteps, parameter_names, IPayoff::GetUpperCap(), IPayoff::GetLowerCap());
		}
		else {
			// In case of no time discretization
			NSteps = 0;
			// Return a tuple tha containes all the outcome information of pricer 
			// To be used from Output class
			return std::make_tuple(m_price, option_data, NSteps, parameter_names, IPayoff::GetUpperCap(), IPayoff::GetLowerCap());
		}
	}

	// Clear methods
	// Are used in case of multipricing, so that the next pricing process will use "empty" vectors without previous information left

	// Clear the vector that holds the parameter names 
	inline void ClearNameParameterVector() {
		parameter_names.clear();
	}

	// Clear the vector that holds the stock flunctuations
	inline void ClearStockFlunctuationsVector() {
		stock_flunct.clear();
	}

	// Clear the vector that holds the option update prices of each simulation
	inline void ClearTempOptionPriceVector() {
		option_prices.clear();
	}

	// Destructor
	~Pricer();
};

#ifndef PRICER_CPP
#include "Pricer.cpp"
#endif // !PRICER_CPP

#endif // !PRICER_HPP
