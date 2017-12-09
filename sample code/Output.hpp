/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ======================================================================
*
*	Monte Carlo Option Pricing Application - Output class
*
*/

// Multiple inclusion guards
#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <fstream>

#include "MIS.hpp"
#include "Pricer.hpp"
#include "MCBuilder.hpp"

// Class that models the output
class Output {
private:
	// Private members to temporary hold the input tuples
	Statistics MIS_Stats;
	PricerResults pricer_results;
	MultiOutputList multi_list;
public:

	// Constructors
	explicit Output();
	explicit Output(const Output & output);
	explicit Output(const PricerResults & pricer_res, const Statistics & mis);

	// Assignment operator
	Output & operator=(const Output & output);
	
	// Selection methods
	void Print(const PricerResults & pricer_res, const Statistics & mis);
	void MultiPrint(MultiOutputList & multi_list_);

	// MultiConsolePrint iterates through the list and calls the corresponding single print function 
	// to each one of the list's information tuples
	inline void MultiConsolePrint() {
		for (auto e : multi_list) {
			Output::ConsolePrint(std::get<0>(e), std::get<1>(e));
			std::cout << "\n\n";
		}
	}

	// MultiTextFileOutput iterates through the list and calls the corresponding single text file output function 
	// to each one of the list's information tuples
	// It also passes a counter to provide output files with different name, i.e. for 5 output files, their name will be "File A", "File B" etc
	// using the multi counter of type char as factor
	inline void MultiTextFileOutput() {
		char multi_counter = 'A';
		for (auto e : multi_list) {
			Output::TextFileOutput(std::get<0>(e), std::get<1>(e), multi_counter);
			multi_counter++;
			std::cout << "\n\n";
		}
	}

	// MultiExcelOutput iterates through the list and calls the corresponding single excel output function 
	// to each one of the list's information tuples
	// It also passes a counter to provide output files with different name, i.e. for 5 output excel files, their name will be "File A", "File B" etc
	// using the multi counter of type char as factor
	inline void MultiExcelOutput() {
		char multi_counter = 'A';
		for (auto e : multi_list) {
			Output::ExcelOutput(std::get<0>(e), std::get<1>(e), multi_counter);
			multi_counter++;
			std::cout << "\n\n";
		}
	}

	// Console print function uses MIS and Pricer information tuples and sends them to the screen 
	inline void ConsolePrint(const PricerResults & pricer_res, const Statistics & mis) {

		// Extract partially the Pricer output
		std::vector<std::string> parameter_names	= std::get<3>(pricer_res);
		OptionData option_data				= std::get<1>(pricer_res);

		std::cout << "\n\n************************** OUTPUT **************************\n\n";

		std::cout << "\n*** Model Parameters ***\n\n";
		std::cout << "1. RNG variate: \t\t"			<< parameter_names[0] << "\n";
		std::cout << "2. FDM Scheme: \t\t\t"		<< parameter_names[1] << "\n";
		std::cout << "3. Underlying derivative: \t" << parameter_names[2] << "\n\n\n\n";

		std::cout << "*** Simulation Results and Statistics ***\n\n";
		std::cout << "MCS Option Price: \t"		<< std::get<0>(pricer_res)	<< " [$]\n\n";
		std::cout << "Mean Stock Price: \t"		<< std::get<0>(mis)			<< "\t[$]\n";
		std::cout << "Max Stock Price: \t"		<< std::get<1>(mis)			<< "\t[$]\n";
		std::cout << "Min Stock Price: \t"		<< std::get<2>(mis)			<< "\t[$]\n\n";
		std::cout << "Standard Deviation: \t"	<< std::get<3>(mis)			<< "\n";
		std::cout << "Standard Error: \t"		<< std::get<4>(mis)			<< "\n";
		std::cout << "Exact Price: \t\t"		<< std::get<5>(mis)			<< " [$]\n";

		std::cout << std::boolalpha;
		std::cout << "Decision:\t\t"			<< std::get<6>(mis)			<< "\n\n";

		std::cout.precision(12);
		std::cout << "Elapsed time of simulation: "		<< std::get<7>(mis) << " seconds\n\n\n\n";

		std::cout << "*** Simulation input parameters ***\n\n";
		std::cout << "Rate of Return: "		<< std::get<1>(option_data) << "\t[%]\n";
		std::cout << "Strike Price: \t"		<< std::get<4>(option_data) << "\t[$]\n";
		std::cout << "Expiry Time: \t"		<< std::get<2>(option_data) << "\t[years]\n";
		std::cout << "Stock Price: \t"		<< std::get<3>(option_data) << "\t[$]\n";
		std::cout << "Volatility: \t"		<< std::get<0>(option_data) << "\t[%]\n\n";
		std::cout << "NSIM: \t\t"			<< std::get<5>(option_data) << "\n";

		if (std::get<2>(pricer_res) != 0) {
			std::cout << "NSteps: \t"		<< std::get<2>(pricer_res)	<< "\n";
		}

		if (abs(std::get<4>(pricer_res) - 0) > 0.1) {
			std::cout << "Option Upper Cap: " << std::get<4>(pricer_res) << "\n";
		}

		if (abs(std::get<5>(pricer_res) - 0) > 0.1) {
			std::cout << "Option Lower Cap: " << std::get<5>(pricer_res) << "\n";
		}
		std::cout << "\n\n*************************************************************\n\n";
	}

	// Excel output function uses MIS and Pricer information tuples and sends them in a newly created excel file 
	inline void ExcelOutput(const PricerResults & pricer_res, const Statistics & mis, char counter) {

		// Create a file instance
		std::ofstream file;

		// Determine the name using the char counter as file number factor
		std::string name = "Monte Carlo Option Pricing ";
		name += counter;

		// Give an extension that indicates excel document creation
		name += ".csv";
		
		// Open the file
		file.open(name);

		// Extract partially the Pricer output
		std::vector<std::string> parameter_names	= std::get<3>(pricer_res);
		OptionData option_data						= std::get<1>(pricer_res);

		// Pass the information in the file with the similar manner we printed it in the screen

		file << "**********************************************************************\n";
		file << "*\n* Final Project for QuantNet's Advanced C++11/C++14 Certificate \n*\n";
		file << "* Monte Carlo Option Pricing \n*\n";
		file << "* Pavlos Sakoglou \n*\n";

		file << "\n\n************************** OUTPUT **************************\n\n";

		file << "\n*** Model Parameters ***\n\n";
		file << "1. RNG variate:,"				<< parameter_names[0] << "\n";
		file << "2. FDM Scheme:,"				<< parameter_names[1] << "\n";
		file << "3. Underlying derivative:,"	<< parameter_names[2] << "\n\n\n\n";

		file << "*** Simulation Results and Statistics ***\n\n";
		file << "MCS Option Price:,"		<< std::get<0>(pricer_res) << ",[$]\n\n";
		file << "Mean Stock Price:,"		<< std::get<0>(mis) << ",[$]\n";
		file << "Max Stock Price:,"			<< std::get<1>(mis) << ",[$]\n";
		file << "Min Stock Price:,"			<< std::get<2>(mis) << ",[$]\n\n";
		file << "Standard Deviation:,"		<< std::get<3>(mis) << "\n";
		file << "Standard Error:,"			<< std::get<4>(mis) << "\n";
		file << "Exact Price:,"				<< std::get<5>(mis) << ",[$]\n";

		if (std::get<6>(mis) == 0) {
			file << "Decision:," << "false" << "\n\n";
		}
		else file << "Decision:," << "true" << "\n\n";


		file << "Elapsed time of simulation:,"	<< std::get<7>(mis)		<< " seconds\n\n\n\n";

		file << "*** Simulation input parameters ***\n\n";
		file << "Rate of Return:, "		<< std::get<1>(option_data) << ",[%]\n";
		file << "Strike Price:,"		<< std::get<4>(option_data) << ",[$]\n";
		file << "Expiry Time:,"			<< std::get<2>(option_data) << ",[years]\n";
		file << "Stock Price:,"			<< std::get<3>(option_data) << ",[$]\n";
		file << "Volatility:,"			<< std::get<0>(option_data) << ",[%]\n\n";
		file << "NSIM:,"				<< std::get<5>(option_data) << "\n";

		if (std::get<2>(pricer_res) != 0) {
			file << "NSteps:," << std::get<2>(pricer_res) << "\n";
		}

		if (abs(std::get<4>(pricer_res) - 0) > 0.1) {
			file << "Option Upper Cap: " << std::get<4>(pricer_res) << "\n";
		}

		if (abs(std::get<5>(pricer_res) - 0) > 0.1) {
			file << "Option Lower Cap: " << std::get<5>(pricer_res) << "\n";
		}

		file << "\n\n***********************************************************\n\n";

		// Close the newly created file
		file.close();

		// Let the user know that the file has been created
		std::cout << "\nFile: " << name << " has been created in the directory!\n";
	}
	

	// Text file output function uses MIS and Pricer information tuples and sends them in a newly created text file 
	inline void TextFileOutput(const PricerResults & pricer_res, const Statistics & mis, char counter) {

		// Create a file instance
		std::ofstream file;

		// Determine the name using the char counter as file number factor
		std::string name = "Monte Carlo Option Pricing ";
		name += counter;

		// Give an extension that indicates text file document creation
		name += ".txt";

		// Open the file
		file.open(name);

		// Extract partially the Pricer output
		std::vector<std::string> parameter_names	= std::get<3>(pricer_res);
		OptionData option_data						= std::get<1>(pricer_res);

		file << "**********************************************************************\n";
		file << "*\n* Final Project for QuantNet's Advanced C++11/C++14 Certificate \n*\n";
		file << "* Monte Carlo Option Pricing \n*\n";
		file << "* Pavlos Sakoglou \n*\n";

		file << "\n\n************************** OUTPUT **************************\n\n";

		file << "\n*** Model Parameters ***\n\n";
		file << "1. RNG variate: \t\t"			<< parameter_names[0] << "\n";
		file << "2. FDM Scheme: \t\t\t"			<< parameter_names[1] << "\n";
		file << "3. Underlying derivative: \t"	<< parameter_names[2] << "\n\n\n\n";

		file << "*** Simulation Results and Statistics ***\n\n";
		file << "MCS Option Price: \t"		<< std::get<0>(pricer_res)	<< " [$]\n\n";
		file << "Mean Stock Price: \t"		<< std::get<0>(mis)			<< "\t[$]\n";
		file << "Max Stock Price: \t"		<< std::get<1>(mis)			<< "\t[$]\n";
		file << "Min Stock Price: \t"		<< std::get<2>(mis)			<< "\t[$]\n\n";
		file << "Standard Deviation: \t"	<< std::get<3>(mis)			<< "\n";
		file << "Standard Error: \t"		<< std::get<4>(mis)			<< "\n";
		file << "Exact Price: \t\t"			<< std::get<5>(mis)			<< " [$]\n";

		if (std::get<6>(mis) == 0) {
			file << "Decision:\t\t" << "false" << "\n\n";
		}
		else file << "Decision:\t\t" << "true" << "\n\n";

		file << "*** Simulation input parameters ***\n\n";
		file << "Rate of Return: "		<< std::get<1>(option_data) << "\t[%]\n";
		file << "Strike Price: \t"		<< std::get<4>(option_data) << "\t[$]\n";
		file << "Expiry Time: \t"		<< std::get<2>(option_data) << "\t[years]\n";
		file << "Stock Price: \t"		<< std::get<3>(option_data) << "\t[$]\n";
		file << "Volatility: \t"		<< std::get<0>(option_data) << "\t[%]\n\n";
		file << "NSIM: \t\t"			<< std::get<5>(option_data) << "\n";

		if (std::get<2>(pricer_res) != 0) {
			file << "NSteps: \t" << std::get<2>(pricer_res) << "\n";
		}

		if (abs(std::get<4>(pricer_res) - 0) > 0.1) {
			file << "Option Upper Cap: " << std::get<4>(pricer_res) << "\n";
		}

		if (abs(std::get<5>(pricer_res) - 0) > 0.1) {
			file << "Option Lower Cap: " << std::get<5>(pricer_res) << "\n";
		}

		file << "\n\n*************************************************************************\n\n";

		// Close the newly created file
		file.close();

		// Let the user know that the file has been created
		std::cout << "\nFile: " << name << " has been created in the directory!\n";
	}
	
	// End of print indicator
	void done();

	// Destructor
	~Output();
};

#endif // !OUTPUT_HPP
