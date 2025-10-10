//CMO

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
using namespace std;

class Borrower {
public:
    string name;
    double loan_amt;
    double rate;
    int time;
    double defaultProb;
    double prepaymentRate;
    vector<double>outstanding_principal;
    vector<double>interest_compo;
    vector<double>actual_pay;           //input actual payments
    vector<double>due;                 //monthly due (if any)
    double monthlyPayment;
    vector<double>actual_standing_principal;       //changes made
    vector<double>actual_interest_compo;
    double interest_arrears;                         //changes made


    Borrower(string n, double a, double r, int t, double d, vector<double>ap) {
        name = n;
        loan_amt = a;
        rate = (r / 12.0) / 100;
        time = t * 12;
        defaultProb = d;
        actual_pay = ap;
        interest_arrears = 0;       //changes made 
        outstanding_principal.push_back(loan_amt);

        double interest1 = (loan_amt * rate);
        interest_compo.push_back(interest1);
        monthlyPayment = loan_amt * (rate * pow(1 + rate, time)) / (pow(1 + rate, time) - 1);
        calculate_payment_splits();
        calculate_due();

        actual_standing_principal = outstanding_principal;               //changes made
        actual_interest_compo = interest_compo;                       //changes made
    }

    void calculate_payment_splits()
    {
        for (int i = 1; i <= time; i++)
        {
            if (outstanding_principal[i - 1] <= 0) {
                outstanding_principal.push_back(0);                                 // if loan is fully paid off...but how to operate it??
                interest_compo.push_back(0);
                continue;
            }

            double current_interest = outstanding_principal[i - 1] * rate;

            interest_compo.push_back(current_interest);

            double reduction_of_principal = monthlyPayment - current_interest;

            double new_principal = outstanding_principal[i - 1] - reduction_of_principal;

            outstanding_principal.push_back(new_principal);

        }


    }

    void calculate_due() {
        due.push_back(0); // due for month 0 is 0   push_back(0);
        for (int i = 1; i <= time; i++)
        {
            double due_amount = actual_pay[i] - monthlyPayment;
            due.push_back(due_amount);
        }

    }



    void calculate_actual_splits(int month) {
        // Ensure vectors are large enough
        if (actual_standing_principal.size() <= month)
            actual_standing_principal.resize(month + 1, 0.0);
        if (actual_interest_compo.size() <= month + 1)
            actual_interest_compo.resize(month + 2, 0.0); // +1 for next month

        double interest_due = actual_interest_compo[month];  // this month's interest

        if (actual_pay[month] >= interest_due) { // prepayment or normal payment
            // Clear arrears if any
            interest_arrears = 0.0;

            double extra = actual_pay[month] - interest_due;

            // Reduce outstanding principal (cap at 0)
            actual_standing_principal[month] = max(0.0, actual_standing_principal[month - 1] - extra);

            // Compute next month's interest
            actual_interest_compo[month + 1] = actual_standing_principal[month] * rate;

        }
        else { // shortfall
            double shortfall = interest_due - actual_pay[month];
            interest_arrears += shortfall;

            // Keep outstanding principal unchanged
            actual_standing_principal[month] = actual_standing_principal[month - 1];

            // Add arrears to next month's interest
            actual_interest_compo[month + 1] = actual_standing_principal[month] * rate + interest_arrears;
        }
    }






};



class Tranche {
public:

    string name;
    double principal;
    double coupon_rate;
    int time;
public:
    vector<double> coupon_payment;           // expected coupon (theoretical)
    vector<double> outstanding_principal;    // expected/outstanding (month 0 present)
    // vector<double> interest_due;

     // actuals (your names)
    vector<double> actual_coupon_payment;
    vector<double> actual_standing_principal;
    vector<double> actual_interest_compo;
    double interest_arrears;


    int priority;  // 1 for A, 2 for B, etc.

    Tranche(string nam, double p, double r, int t, int pr) {
        name = nam;
        principal = p;
        coupon_rate = (r / 12.0) / 100;
        time = t * 12;
        priority = pr;

        // initialize theoretical vectors
        coupon_payment.push_back(0);               // month 0
        outstanding_principal.push_back(principal);
        // interest_due.push_back(0);


         // initialize actual vectors (month 0)
        actual_coupon_payment.push_back(0);
        actual_standing_principal.push_back(principal);
        actual_interest_compo.push_back(0);
        interest_arrears = 0.0;
    }

    // 1) Fill expected coupon payments for months 1..months.
    //    This uses outstanding_principal[month-1] — so
    //    call it after you have a projected outstanding_principal schedule
    //    OR call it each month with months = current_month to compute on the fly.
// put inside class Tranche
// Inside class Tranche

    double expected_coupon_due(int month) {
        double base_out;
        if (actual_standing_principal.size() > (size_t)(month - 1))
            base_out = actual_standing_principal[month - 1];
        else
            base_out = outstanding_principal[month - 1];

        double expected_value = base_out * coupon_rate;

        // --- THE FIX IS HERE ---
        // 1. ADD THIS CHECK to see if the vector is big enough
        if (coupon_payment.size() <= (size_t)month) {
            // 2. If not, resize it to make space
            coupon_payment.resize(month + 1, 0.0);
        }

        // 3. Now it is SAFE to assign the value
        coupon_payment[month] = expected_value;
        // ----------------------

        return expected_value;
    }




    // 2) Apply actual amounts received from the pool for the month.
    //    coupon_received = amount paid from interest pool to this tranche for month
    //    principal_received = amount paid from principal pool to this tranche for month
    void apply_actual_payment(int month, double coupon_received, double principal_received) {
        // ensure actual vectors have room for index 'month'
        if (actual_standing_principal.size() <= (size_t)month)
            actual_standing_principal.resize(month + 1, 0.0);
        if (actual_interest_compo.size() <= (size_t)month + 1)
            actual_interest_compo.resize(month + 2, 0.0);
        if (actual_coupon_payment.size() <= (size_t)month)
            actual_coupon_payment.resize(month + 1, 0.0);
        if (coupon_payment.size() <= (size_t)month)
            coupon_payment.resize(month + 1, 0.0); // ensure expected exists (0 if not set)

        double expected_interest = coupon_payment[month];                // current month's scheduled coupon
        double total_interest_due = expected_interest + interest_arrears; // include past arrears

        // pay interest (arrears + current) from coupon_received
        double interest_paid_total = min(coupon_received, total_interest_due);

        // amount that goes to current month's coupon (pay current first or arrears first?).
        // We'll apply to current coupon first (common), then to arrears.
        double to_current_coupon = min(interest_paid_total, expected_interest);
        double to_arrears_paid = interest_paid_total - to_current_coupon;

        actual_coupon_payment[month] = to_current_coupon;
        actual_interest_compo[month] = to_current_coupon; // what investors see as coupon for month

        // update arrears = previously due - paid toward arrears (remaining unpaid)
        interest_arrears = total_interest_due - interest_paid_total; // >= 0

        // principal payment: reduce outstanding by principal_received (cap at 0)
        double prev_out = actual_standing_principal[month - 1];
        double new_out = prev_out - principal_received;
        if (new_out < 0.0) new_out = 0.0;
        actual_standing_principal[month] = new_out;

        // set next month's actual expected interest (interest on new outstanding + arrears)
        actual_interest_compo[month + 1] = actual_standing_principal[month] * coupon_rate + interest_arrears;
    }

};



class Investor {
public:
    string name;
    Tranche* tranche_ptr;   // pointer to the tranche
    double fraction;        // share (0.0 to 1.0)

    vector<double> coupon_received;
    vector<double> principal_received;

    Investor(string n, Tranche* t, double frac) {
        name = n;
        tranche_ptr = t;   // store address of the tranche
        fraction = frac;
        coupon_received.push_back(0);
        principal_received.push_back(0);
    }

    void record_payment(int month) {
        if (!tranche_ptr) return;
        if (month <= 0) return; // disallow month 0

        if (coupon_received.size() <= (size_t)month)
            coupon_received.resize(month + 1, 0.0);
        if (principal_received.size() <= (size_t)month)
            principal_received.resize(month + 1, 0.0);

        // check tranche vectors exist for this month
        if (tranche_ptr->actual_coupon_payment.size() <= (size_t)month) {
            cout << "No coupon data for month " << month << " yet.\n";
            return;
        }
        if (tranche_ptr->actual_standing_principal.size() <= (size_t)month) {
            cout << "No principal data for month " << month << " yet.\n";
            return;
        }

        coupon_received[month] = tranche_ptr->actual_coupon_payment[month] * fraction;
        principal_received[month] = (tranche_ptr->actual_standing_principal[month - 1]
            - tranche_ptr->actual_standing_principal[month]) * fraction;

        cout << "Investor " << name << " received in month " << month << ": Coupon = "
            << coupon_received[month] << ", Principal = " << principal_received[month] << endl;
    }

};
















void runCMO() {





    // Input borrowers

    int no_of_borrowers;
    cout << "Enter number of borrowers: ";
    cin >> no_of_borrowers;
    vector<Borrower> borrowers;
    for (int i = 0; i < no_of_borrowers; i++) {
        string name;
        double loan_amt, rate, defaultProb;
        int time;
        vector<double>ap;
        cout << "Enter details for borrower " << i + 1 << endl;
        cout << "name : ";
        cin >> name;
        cout << endl;
        cout << "loan amt: ";
        cin >> loan_amt;
        cout << endl;
        cout << "rate:";
        cin >> rate;
        cout << endl;
        cout << "time (in years): ";
        cin >> time;
        cout << endl;
        cout << "default probability (%): ";
        cin >> defaultProb;
        cout << endl;
        cout << "Enter actual payments for each month (space-separated, total " << time * 12 << " values): ";   //keep the 0th index of the input array of ap as 0
        ap.push_back(0); // month 0 payment is 0
        for (int j = 0; j < time * 12; j++) {
            double pay;
            cin >> pay;
            ap.push_back(pay);
        }
        Borrower B(name, loan_amt, rate, time, defaultProb / 100.0, ap);
        borrowers.push_back(B);
        cout << endl;
    }

    cout << "-------------borrowers entered successfully---------------" << endl;
    // ___________________________________________________
//////////////////////////////////////////////////////////////////////////////////////////////


    cout << "Enter 1 to access the borrowers expected amortization schedule else press 0 to continue: ";
    int access;
    cin >> access;
    cout << endl;
    if (access == 1)
    {
        cout << "Enter the borrower index number you want to access (0-based) : ";
        int index;
        cin >> index;
        cout << endl;





    }


    //Tranches:

    int months;
    cout << "Enter the no of  months: ";
    cin >> months;
    cout << "-----------now enter below-----------" << endl;

    // Input tranches
    int num_tranches;
    cout << "Enter number of tranches: ";
    cin >> num_tranches;

    vector<Tranche> tranches;

    for (int i = 0; i < num_tranches; i++) {
        string name;
        double principal, rate;
        int time, priority;

        cout << "Enter details for tranche " << i + 1
            << " (name, principal, annual coupon rate %, time in years, priority): ";
        cin >> name >> principal >> rate >> time >> priority;

        Tranche T(name, principal, rate, time, priority);
        tranches.push_back(T);
    }

    // Sort tranches once after input
    sort(tranches.begin(), tranches.end(), [](const Tranche& a, const Tranche& b) {  return a.priority < b.priority; });     //lambda function for sorting  OIOIOIOI


    cout << "-------------Tranches set up successfully---------------" << endl;





    ////////////////////////////////////////////////////////////////////////////////////////////////////
                                                   //investors:

    int num_investors;
    cout << "Enter number of investors: ";
    cin >> num_investors;
    vector<Investor> investors;
    for (int i = 0; i < num_investors; i++) {
        string name;
        double fraction;
        cout << "Enter details for investor " << i + 1 << " (name): ";
        cin >> name;
        cout << "there are" << tranches.size() << " tranches with indices 0 to " << tranches.size() - 1 << " in priority order" << endl;
        cout << "so enter the tranche index you want to choose" << endl;
        int tranche_index;
        cin >> tranche_index;
        cout << "Enter fraction of tranche to invest in (0.0 to 1.0): ";
        cin >> fraction;

        Investor inv(name, &tranches[tranche_index], fraction);
        investors.push_back(inv);
    }
    cout << "-------------investors entered successfully---------------" << endl;
    // ___________________________________________________
    ////////////////////////////////////////////////////////////////////////////////////////////////////


      //----------------------------------------------
     // operations between borrowers and tranches    |
    //------------------------------------------------



    for (int i = 1; i <= months; ++i)
    {
        double interest_pool = 0.0;
        double principal_pool = 0.0;
        for (int b = 0; b < borrowers.size(); b++)
        {
            borrowers[b].calculate_actual_splits(i);
            //interest_pool += borrowers[b].actual_interest_compo[i];
            double interest_due_this_month = borrowers[b].actual_interest_compo[i];
            double interest_actually_paid = min(borrowers[b].actual_pay[i], interest_due_this_month);

            interest_pool += interest_actually_paid;
            principal_pool += (borrowers[b].actual_standing_principal[i - 1] - borrowers[b].actual_standing_principal[i]);
        }




        // --- WATERFALL LOGIC BEGINS ---

        double remaining_interest = interest_pool;
        double remaining_principal = principal_pool;

        // --- 1. Distribute INTEREST to all tranches first ---
        // This loop gives each tranche its share of the collected interest.
        for (size_t t = 0; t < tranches.size(); t++) {
            if (remaining_interest <= 0) {
                // If the interest pool is empty, we still need to call apply_actual_payment
                // to correctly carry forward the full interest due as arrears.
                tranches[t].expected_coupon_due(i); // This ensures coupon_payment[i] is set
                tranches[t].apply_actual_payment(i, 0.0, 0.0);
                continue;
            }

            // Calculate the total interest owed to this tranche (new interest + old arrears)
            double new_interest = tranches[t].expected_coupon_due(i);
            double total_interest_due = new_interest + tranches[t].interest_arrears;

            // Determine how much of the remaining interest pool to pay
            double interest_to_pay = min(remaining_interest, total_interest_due);

            // Apply the payment. Note that we pass 0.0 for the principal payment for now.
            tranches[t].apply_actual_payment(i, interest_to_pay, 0.0);

            remaining_interest -= interest_to_pay;
        }

        // --- 2. Distribute PRINCIPAL sequentially ---
        // This loop gives the highest-priority tranche all available principal until it is paid off.
        for (size_t t = 0; t < tranches.size(); t++) {
            if (remaining_principal <= 0) {
                break; // No more principal to distribute
            }

            // The outstanding principal for this tranche (after the interest-only payment)
            double principal_outstanding = tranches[t].actual_standing_principal[i];

            // Determine how much principal to pay this tranche
            double principal_to_pay = min(remaining_principal, principal_outstanding);

            // Directly subtract the principal payment from this month's balance
            tranches[t].actual_standing_principal[i] -= principal_to_pay;

            remaining_principal -= principal_to_pay;


        }
        // After principal sequential distribution finished, update next-month interest for all tranches:
        for (size_t t = 0; t < tranches.size(); ++t) {
            // ensure vector space
            if (tranches[t].actual_interest_compo.size() <= (size_t)(i + 1))
                tranches[t].actual_interest_compo.resize(i + 2, 0.0);

            // recompute based on the current actual outstanding and arrears
            tranches[t].actual_interest_compo[i + 1] = tranches[t].actual_standing_principal[i] * tranches[t].coupon_rate + tranches[t].interest_arrears;
        }


        // --- WATERFALL LOGIC ENDS ---

    }



    //----------------------------------------------
   // operations between tranches and investors:    |
  //------------------------------------------------

    int stop = 1;
    while (stop != 0)
    {
        cout << "enter the investor's name to access it's monthly receipt: ";
        string investor;
        cin >> investor;
        cout << endl;
        cout << "enter the month number of the receipt: ";
        int month;
        cin >> month;
        cout << endl;
        for (int i = 0; i < investors.size(); i++)
        {
            if (investors[i].name == investor)
            {
                investors[i].record_payment(month);

                break;
            }
        }

        int stop_switch;
        cout << "enter 0 to stop or any other number to continue: ";
        cin >> stop_switch;
        stop = stop_switch;
    }



}
