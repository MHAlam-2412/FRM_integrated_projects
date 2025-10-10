//VAREngine.CPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
using namespace std;


void VaR()
{
    cout << "To get a simulation based VaR for your data set, Please provide the 'complete' path of your dataset as" << endl;
    cout << "(Note: The dataset should contain a single column of returns.)" << endl;
    string file_path;
    cin >> file_path;


    ifstream file(file_path);
    vector<double> returns;
    double value;

    if (!file) {
        cout << "Error opening file" << endl;
        return;
    }

    while (file >> value) {
        returns.push_back(value);
    }

    file.close();

    // Test read
    for (double r : returns) {
        cout << r << endl;
    }

    //calculate mean
    double sum = 0;
    for (int i = 0; i < returns.size(); i++)
    {
        sum += returns[i];
    }

    double mean = sum / returns.size();

    // calculate standard deviation
    double sum_squared_diff = 0;
    for (int i = 0; i < returns.size(); i++)
    {
        sum_squared_diff += (returns[i] - mean) * (returns[i] - mean);
    }

    double stddev = sqrt(sum_squared_diff / (returns.size() - 1));

    int n;
    cout << "Enter the number of simulations: ";
    cin >> n;
    cout << endl;


    // Monte Carlo simulation
    random_device rd;

    mt19937 gen(rd());

    normal_distribution<double> dist(mean, stddev);

    vector<double> simulations(n);

    for (int i = 0; i < n; i++) {
        simulations[i] = dist(gen);
    }

    sort(simulations.begin(), simulations.end());

    cout << "Simulated returns: " << endl;
    for (int i = 0; i < n; i++)
    {
        cout << simulations[i] << endl;
    }

    cout << "--------------------------------------------------------------------------------------" << endl;

    int percentile;
    cout << "Enter the percentile for VaR (e.g., 95 for 95%): ";
    cin >> percentile;
    cout << endl;


    if (percentile < 0 || percentile > 100) {
        cout << "Invalid percentile value." << endl;
        return;
    }

    int index = ((100 - percentile) / 100) * n;

    double var = simulations[index + 1];
    cout << "Value at Risk (VaR) at " << percentile << "% confidence level: " << var << endl;


    // Expected Shortfall (ES) calculation
    double expected_shortfall = 0;
    for (int i = 0; i <= index; i++) {
        expected_shortfall += simulations[i];
    }
    expected_shortfall /= (index + 1);
    cout << "Expected Shortfall (ES) at " << percentile << "% confidence level: " << expected_shortfall << endl;
    cout << "Simulation completed." << endl;


}