# Multi-threaded Monte Carlo Simulation App for Option Pricing in C++11/14

# Description 

The Monte Carlo Option Pricing application is used in computational finance to approximate prices of financial derivatives (options) given a set of parameters as input, either determined by the user in real-time, either having this set of parameters fixed, or fed into the program from another system.

The current application follows a classification logic, that is, it groups the model parameters into data aggregates and then uses them into the pricing algorithm that implements the Monte Carlo simulation, which prices them. When the pricing is done, the outcome along with the simulation data are sent into a management information class that computes certain statistics on the pricing method that was used, in order to help the user to evaluate the whole process. Finally, the data and the newly computed statistics are send to an output class, with which the user can choose to print them on the console, or to save them in a .txt file, or in an excel document.

In case the user wants to price multiple derivatives at once, the application provides an extra feature that allows the user to choose once the model parameters and select multiple option contracts to price. The application saves the results of each pricing process, which will then be printed all together in the same way, by either printing in the console consecutively, or creating multiple files.


# Usage

Download the .exe file in your computer and then run it. Use a virtual machine in case you operate in Mac OS, or Wine for other operating systems than Windows: https://www.winehq.org/

Since the code is for demonstration only, there are missing components of the application, thus you cannot compile the provided files in this repository, except the plain Monte Carlo file. 

# System Components

**FDM_SDE class**

The user can choose between three numerical approximations for the pricing process, namely, the general Geometric Brownian Motion, the Explicit Euler Approximation, and the Milstein Approximation. All methods work equally well, however, in regards to computational speed and convergence, the General Geometric Brownian Motion is the fastest one, with the Explicit Euler close second, and finally the Milstein approximation, which appears to have slightly slower computational convergence.

**RNG class**

The random generation processes used in the application are the Default Random Engine and the Mersenne Twister Engine, coupled with a standard normal distribution variate. The user can choose either engine in run-time, or fix a random engine before the compilation. For more random generation processes, the application is easy to extend, namely, the developer needs to add an extra method by modifying the RNG class and its user interface appropriately. 

**Payoff class**
The option contracts that are subject to pricing in this application are the following:
  •	European Call/Put
  •	Asian Call/Put
  •	Knock-out Call/Put
  •	Knock-in Call/Put
The process of extending the application into pricing more option contracts, is again simple. The user has to define the extra payoff functions and modify the corresponding user interface, in Payoff class. Alternatively, the user can hard code a new payoff and pass it as argument either in the pricing class Pricer, or via Payoff class setters.

**Input class**

The input class provides a basic interface, user-interactive, for option data selection in run-time, along with basic construction and initialization methods, one setter and getter for group data retrieve, and of course the option data parameters as public members, so that to be accessible without the need of extra member functions.

**Pricer class**
The Pricer class is a templated class, using as template parameters the four classes that are described above: RNG, Input, FDM_SDE, Payoff. Its purpose is to use user-determined input, either in run time or pre-determined, and use it to price accordingly the derivative as per the appropriate model. Therefore, it implements a general pricer function, that can also handle the Asian options by averaging the stock price in [0,T] and use it in the payoff if certain conditions are satisfied.

Moreover, the Pricer has setters for the model parameters, but also implements a get() method that calls the user-interactive interface of the parameter classes, and determines all the model data. Then, it initializes its members appropriately and feeds them to the pricing algorithms accordingly. When the process is over, it gathers the output data into tuples and provides getter function so that they can be used in other classes such as MIS and Output.

**MIS class**
This class serves only for managerial decisions and produces statistics given the simulation data. It computes the mean, max and min prices of the random process, standard deviation and standard error, etc. Moreover, it computes the exact prices of the underlying derivative and compares it with the approximated price. If the approximated price is close enough to the exact price, it indicates “true” as a decision, otherwise “false”. The user then can decide if the simulation is successful or satisfying enough.
Furthermore, MIS class provides a stopwatch method that measures the processing time of the pricing algorithm, providing useful information on a system performance scale. 

**Output class**
This class takes input from MIS and Pricer class and provides a user-interactive interface for printing method selection. The printing options are the console, a text file, and an excel file. Moreover, in case of multiple option pricing, there is an appropriate printing interface that iterates a list of multiple Pricer and MIS output data, and prints them consecutively in one of the above methods.

**Builder class**
The builder class servers as a mediator method that puts the whole process together. It is a meta-programming class with three methods, namely, an informative message method, a run() method, that allows the user to select the pricing model and parameters, as well as the number of options to be priced, the pricing methods, the MIS methods, and finally the output methods. 

The motivation for a builder class is to keep the main() function cleaner. As for performance issues, we gain very little by implementing the functionality directly in main() without a mediator method. One would argue that the program becomes “heavier” with extra classes, however, the next-generation pattern is quite appropriate for such code design. Finally, the Builder provides a bye() method that indicates the end of the program, after the output print is completed.

# Design and domain architecture 

The system architecture is based entirely on meta-programming and next generation pattern, where the data are interchanged between the classes via function wrappers, tuples and information containers, and direct calls of static and inline functions. Thus, there are no class hierarchies and the object creation and copying is kept at the minimum. 
The builder is based on the concept of MAN domain architecture since its purpose is the object creation and data assembly between the system’s classes. The Pricer, however, is based on the concept of RAT, since it tracks and allocates information in run-time (optionally with fixed data) through the system. 

UML diagram

![UML](http://url/to/img.png)

Furthermore, the data flow diagram below describes how the information is distributed throughout the system’s components:

Data flow diagram
 
![Data-Flow](http://url/to/img.png)



