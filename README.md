# FRM_integrated_projects:

# CMO Cash Flow & Waterfall Simulator (C++)

## Overview
This project is a **Collateralized Mortgage Obligation (CMO) cash-flow simulator** written in modern C++.  
It models how **borrower-level mortgage cash flows** are transformed, pooled, and distributed across **structured tranches**, and finally passed on to **investors** based on their tranche exposure.

The simulator is **stateful and dynamic**: all interest, principal, and arrears are updated **month-by-month using actual borrower payments**, not just theoretical schedules.  
This makes the project closer to real-world structured finance behavior than static amortization models.

---

## Core Idea
> Borrowers → Cash Flow Pool → Tranche Waterfall → Investors

- Borrowers generate **monthly interest and principal cash flows**
- These are pooled into **interest and principal pools**
- Cash flows are distributed via a **priority-based waterfall**
- Investors receive payments proportional to their **fractional tranche ownership**

---

## Key Components & Design

### 1. Borrower Module
Each borrower represents an individual mortgage with:
- Loan amount, interest rate, and tenure
- Full **amortization schedule**
- **Actual monthly payment stream** (can differ from scheduled payment)
- Tracking of:
  - Outstanding principal
  - Interest due
  - Interest arrears

**Important feature (USP):**  
Borrowers are updated using **actual payments**, allowing:
- Prepayments
- Shortfalls
- Interest arrears accumulation
- Dynamic principal reduction

This mirrors real mortgage behavior instead of assuming perfect payments.

---

### 2. Tranche Module
Each tranche represents a structured debt layer with:
- Principal amount
- Coupon rate
- Maturity
- Priority (seniority)

Tranches maintain:
- Expected coupon schedule
- Actual coupon received
- Actual outstanding principal
- Interest arrears

**Waterfall logic:**
1. Interest is paid **first**, in priority order
2. Unpaid interest is carried forward as arrears
3. Principal is distributed **sequentially**, highest priority first
4. All balances and future interest are recalculated dynamically

---

### 3. Investor Module
Investors are linked to tranches via:
- A pointer-based association
- Fractional ownership (0–1)

Each investor receives:
- Proportional coupon payments
- Proportional principal repayments
- Month-wise cash flow records

This creates a clean abstraction between **instrument structure (tranches)** and **ownership (investors)**.

---

## Simulation Flow (End-to-End)

1. Input borrower data and actual monthly payments
2. Generate borrower amortization schedules
3. Initialize tranches and assign priorities
4. Assign investors to tranches
5. For each month:
   - Update borrower cash flows using actual payments
   - Pool interest and principal
   - Apply tranche waterfall logic
   - Update tranche balances and arrears
6. Query investor-level cash flows for any month

---

## Data Structures & DSA Concepts Used

### Object-Oriented Design
- Encapsulation of financial entities (`Borrower`, `Tranche`, `Investor`)
- Clear separation of responsibilities
- Pointer-based relationships for efficient linkage

### STL Containers
- `vector<double>` for time-series cash flows
- Dynamic resizing for variable-length simulations
- Safe indexing patterns to handle evolving states

### Algorithms & Logic
- Priority-based sorting of tranches (`std::sort` + lambda)
- Min/max bounding for cash flow allocation
- Iterative state updates across time steps

### Memory & Performance
- No redundant recomputation of schedules
- Forward-only monthly updates
- Efficient reuse of stored states

---

## Why This Project Is Not Just a Toy Model
- Uses **actual payments**, not assumed schedules
- Supports arrears, shortfalls, and prepayments
- Implements a realistic **interest-first waterfall**
- Separates economic logic from ownership logic
- Designed for extension (defaults, recoveries, stress testing)

---

## Input Design Note (Intentional Minimalism)
The current implementation uses **interactive console input** for:
- Borrowers
- Tranches
- Investors
- Monthly payments

This keeps the core simulation logic **transparent and debuggable**, especially during development and validation.

That said, the architecture is already compatible with:
- Batch-based input pipelines
- Structured datasets
- External data sources

*(Readers familiar with production financial systems will recognize where this naturally evolves next.)*

---

## Limitations & Planned Enhancements
- Automated CSV / file-based input pipeline
- Probabilistic default and recovery modeling
- Prepayment modeling using CPR/PSA
- Tranche-level metrics (WAL, IRR, duration)
- Scenario and stress testing
- Performance optimization for large portfolios

---

## Use Cases
- Structured finance learning & experimentation
- C++ OOP and DSA demonstration
- Cash-flow modeling practice
- Financial engineering prototyping

---

## Author Notes
This project was built to emphasize **clarity of cash-flow mechanics**, **correct state transitions**, and **realistic financial behavior**, rather than UI or data ingestion layers.

The focus is on **how money actually moves**.


By
  Md Hamid Alam
---

## Build & Run
```bash
g++ -std=c++17 main.cpp -o cmo_sim
./cmo_sim
----------


