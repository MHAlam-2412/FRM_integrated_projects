#ifndef CMO_H
#define CMO_H

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// ---------- Borrower Class ----------
class Borrower {
public:
    string name;
    double loan_amt;
    double rate;
    int time;
    double defaultProb;
    double prepaymentRate;
    vector<double> outstanding_principal;
    vector<double> interest_compo;
    vector<double> actual_pay;
    vector<double> due;
    double monthlyPayment;
    vector<double> actual_standing_principal;
    vector<double> actual_interest_compo;
    double interest_arrears;

    Borrower(string n, double a, double r, int t, double d, vector<double> ap);
    void calculate_payment_splits();
    void calculate_due();
    void calculate_actual_splits(int month);
};

// ---------- Tranche Class ----------
class Tranche {
public:
    string name;
    double principal;
    double coupon_rate;
    int time;
    vector<double> coupon_payment;
    vector<double> outstanding_principal;
    vector<double> actual_coupon_payment;
    vector<double> actual_standing_principal;
    vector<double> actual_interest_compo;
    double interest_arrears;
    int priority;

    Tranche(string nam, double p, double r, int t, int pr);
    double expected_coupon_due(int month);
    void apply_actual_payment(int month, double coupon_received, double principal_received);
};

// ---------- Investor Class ----------
class Investor {
public:
    string name;
    Tranche* tranche_ptr;
    double fraction;
    vector<double> coupon_received;
    vector<double> principal_received;

    Investor(string n, Tranche* t, double frac);
    void record_payment(int month);
};

// ---------- Main Function ----------
void runCMO();

#endif
#pragma once
