# Auto Wash Management System

A console-based car wash management system built in C 
for Trish & Shaun's Auto Wash Services. Manages customer 
intake, wash bay capacity, payments, and daily sales reporting.

## Features
- Customer registration with license plate validation
  (Jamaican format: 4 digits + up to 2 uppercase letters)
- 6 service types with individual pricing via enum
- Wash bay capacity management (max 3 cars simultaneously)
- Duplicate license plate detection
- Payment processing: Cash and Card (3% card fee applied)
- Automatic 2% tip calculation
- Binary file persistence — data survives program restart
- Receipt generation per customer
- Daily sales report with payment method breakdown
- Returning customer display

## Tech Stack
- Language: C
- Storage: Binary file I/O (fwrite/fread)
- Data Modeling: Structs and Enums
- Date/Time: time.h for automatic transaction dating

## How to Run
1. Compile: gcc carwash.c -o carwash
2. Run: ./carwash

## Author
Kay-Ann Green — github.com/Kay-Ann14
