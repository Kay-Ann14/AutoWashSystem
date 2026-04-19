#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_CUSTOMERS 20
#define MAX_SERVICES 5
#define WASH_BAY_CAPACITY 3  // Maximum number of cars that can be washed at a time

// Enum for services
typedef enum {
    WASH_VACUUM = 2500,
    ENGINE_WASH = 2000,
    POLISHING = 3500,
    BUFFING = 5500,
    ROOF_CLEANING = 3200,
    DETAILING = 7500
} ServiceCost;

// Structure to hold customer data
typedef struct {
    char firstName[50];
    char lastName[50];
    char licensePlate[7];  // License plate should be 4 digits and 2 optional letters
    int service[MAX_SERVICES];  // Services chosen by the customer
    int serviceCount;  // Number of services chosen
    int paymentMethod;  // 1: Cash, 2: Card
    float totalCost;  // Total service cost
    float tip;  // Tip for the customer
    char transactionDate[11];  // Transaction date in dd/mm/yyyy format
} Customer;

// Structure to track the current wash bay status
typedef struct {
    char licensePlate[7];
    int isWashing;
} WashBay;

// Global array to hold customer data and wash bay status
Customer customers[MAX_CUSTOMERS];
int customerCount = 0;  // Counter for the number of customers
WashBay washBay[WASH_BAY_CAPACITY];  // Array to hold the current cars in the wash bay

// Function prototypes
void displayMenu();
void addCustomer();
void washCar();
void displayReturningCustomers();
void displaySalesReport();
void exitProgram();
int isValidLicensePlate(const char* licensePlate);
int isLicensePlateUnique(const char* licensePlate);
float calculateServiceCost(int services[], int serviceCount, int paymentMethod);
void saveToFile(Customer customer);
void loadCustomersFromFile();
void displayReceipt(Customer customer);
void clearWashBay();
int isWashBayFull();
int isPlateInWashBay(const char* licensePlate);
void updateWashBay(const char* licensePlate, int status);

// Main program
int main() {
    loadCustomersFromFile();  // Load customer data from file
    memset(washBay, 0, sizeof(washBay));  // Clear the wash bay status at the start

    while (1) {
        displayMenu();  // Display the menu
        int choice;
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addCustomer(); break;
            case 2: washCar(); break;
            case 3: displayReturningCustomers(); break;
            case 4: displaySalesReport(); break;
            case 5: exitProgram(); return 0;
            default: printf("Invalid choice. Please try again.\n"); break;
        }
    }
    return 0;
}

// Display menu options
void displayMenu() {
    printf("\n--- Trish & Shaun's Auto Wash Services ---\n");
    printf("1. Add Customer and Process Payment\n");
    printf("2. Wash Car\n");
    printf("3. Display Returning Customers\n");
    printf("4. Display Day's Sales Report\n");
    printf("5. Exit Program\n");
    printf("------------------------------------------\n");
}

// Add customer information
void addCustomer() {
    if (customerCount >= MAX_CUSTOMERS) {
        printf("Car wash is at full capacity. Cannot add more customers.\n");
        return;
    }

    Customer newCustomer;
    printf("Enter First Name: ");
    scanf("%s", newCustomer.firstName);
    printf("Enter Last Name: ");
    scanf("%s", newCustomer.lastName);

    // Get license plate number and validate
    printf("Enter Vehicle License Plate (4 digits and 2 letters): ");
    scanf("%s", newCustomer.licensePlate);
    
    if (!isValidLicensePlate(newCustomer.licensePlate)) {
        printf("Invalid license plate format.\n");
        return;
    }

    if (!isLicensePlateUnique(newCustomer.licensePlate)) {
        printf("This license plate number is already in use.\n");
        return;
    }

    // Get services chosen by the customer
    printf("Enter number of services (1-5): ");
    scanf("%d", &newCustomer.serviceCount);
    for (int i = 0; i < newCustomer.serviceCount; i++) {
        printf("Enter service %d (1: Wash and Vacuum, 2: Engine Wash, 3: Polishing, 4: Buffing, 5: Roof Cleaning, 6: Detailing): ", i+1);
        scanf("%d", &newCustomer.service[i]);
    }

    // Get payment method (1: Cash, 2: Card)
    printf("Enter Payment Method (1: Cash, 2: Card): ");
    scanf("%d", &newCustomer.paymentMethod);

    // Calculate total cost and tip
    newCustomer.totalCost = calculateServiceCost(newCustomer.service, newCustomer.serviceCount, newCustomer.paymentMethod);
    newCustomer.tip = newCustomer.totalCost * 0.02;  // 2% tip
    newCustomer.totalCost += newCustomer.tip;

    // Set current date
    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);
    strftime(newCustomer.transactionDate, sizeof(newCustomer.transactionDate), "%d/%m/%Y", tm_info);

    // Save customer details to file
    saveToFile(newCustomer);

    // Display receipt
    displayReceipt(newCustomer);
}

// Validate license plate format (4 digits, 2 optional letters)
int isValidLicensePlate(const char* licensePlate) {
    if (strlen(licensePlate) < 4 || strlen(licensePlate) > 6) return 0;

    for (int i = 0; i < 4; i++) {
        if (!isdigit(licensePlate[i])) return 0;
    }

    if (strlen(licensePlate) == 5 || strlen(licensePlate) == 6) {
        if (!isupper(licensePlate[4]) || (strlen(licensePlate) == 6 && !isupper(licensePlate[5]))) {
            return 0;
        }
    }

    return 1;
}

// Check if the license plate is unique
int isLicensePlateUnique(const char* licensePlate) {
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].licensePlate, licensePlate) == 0) {
            return 0;
        }
    }
    return 1;
}

// Calculate total cost of services
float calculateServiceCost(int services[], int serviceCount, int paymentMethod) {
    float totalCost = 0;
    for (int i = 0; i < serviceCount; i++) {
        switch (services[i]) {
            case 1: totalCost += WASH_VACUUM; break;
            case 2: totalCost += ENGINE_WASH; break;
            case 3: totalCost += POLISHING; break;
            case 4: totalCost += BUFFING; break;
            case 5: totalCost += ROOF_CLEANING; break;
            case 6: totalCost += DETAILING; break;
            default: break;
        }
    }

    // Apply card payment fee (3%)
    if (paymentMethod == 2) {
        totalCost += totalCost * 0.03;
    }

    return totalCost;
}

// Save customer data to file
void saveToFile(Customer customer) {
    FILE *file = fopen("customers.dat", "ab");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fwrite(&customer, sizeof(Customer), 1, file);
    fclose(file);
    customerCount++;
}

// Load customer data from file
void loadCustomersFromFile() {
    FILE *file = fopen("customers.dat", "rb");
    if (file == NULL) return;

    while (fread(&customers[customerCount], sizeof(Customer), 1, file)) {
        customerCount++;
    }
    fclose(file);
}

// Display customer receipt
void displayReceipt(Customer customer) {
    printf("\n--- Receipt ---\n");
    printf("Customer: %s %s\n", customer.firstName, customer.lastName);
    printf("License Plate: %s\n", customer.licensePlate);
    printf("Services Chosen: ");
    for (int i = 0; i < customer.serviceCount; i++) {
        switch (customer.service[i]) {
            case 1: printf("Wash and Vacuum "); break;
            case 2: printf("Engine Wash "); break;
            case 3: printf("Polishing "); break;
            case 4: printf("Buffing "); break;
            case 5: printf("Roof Cleaning "); break;
            case 6: printf("Detailing "); break;
            default: break;
        }
    }
    printf("\nTotal Cost: $%.2f\n", customer.totalCost);
    printf("Tip: $%.2f\n", customer.tip);
    printf("Payment Method: %s\n", (customer.paymentMethod == 1 ? "Cash" : "Card"));
    printf("Transaction Date: %s\n", customer.transactionDate);
    printf("-------------------\n");
}

// Function to simulate washing the car
void washCar() {
    char licensePlate[7];
    printf("Enter Vehicle License Plate: ");
    scanf("%s", licensePlate);

    if (isPlateInWashBay(licensePlate)) {
        printf("The vehicle with license plate %s is already being washed.\n", licensePlate);
        return;
    }

    if (isWashBayFull()) {
        printf("The wash bay is full. Please wait until space is available.\n");
        return;
    }

    // Find an available spot in the wash bay and update the status
    for (int i = 0; i < WASH_BAY_CAPACITY; i++) {
        if (washBay[i].isWashing == 0) {
            strcpy(washBay[i].licensePlate, licensePlate);
            washBay[i].isWashing = 1;
            printf("Vehicle %s is now being washed.\n", licensePlate);
            return;
        }
    }
}

// Check if the wash bay is full
int isWashBayFull() {
    for (int i = 0; i < WASH_BAY_CAPACITY; i++) {
        if (washBay[i].isWashing == 0) {
            return 0;  // There is space in the wash bay
        }
    }
    return 1;  // No space available
}

// Check if a vehicle is in the wash bay
int isPlateInWashBay(const char* licensePlate) {
    for (int i = 0; i < WASH_BAY_CAPACITY; i++) {
        if (washBay[i].isWashing && strcmp(washBay[i].licensePlate, licensePlate) == 0) {
            return 1;  // The vehicle is in the wash bay
        }
    }
    return 0;  // The vehicle is not in the wash bay
}

// Update the wash bay status
void updateWashBay(const char* licensePlate, int status) {
    for (int i = 0; i < WASH_BAY_CAPACITY; i++) {
        if (strcmp(washBay[i].licensePlate, licensePlate) == 0) {
            washBay[i].isWashing = status;  // Update wash status
            break;
        }
    }
}

// Display returning customers
void displayReturningCustomers() {
    if (customerCount == 0) {
        printf("No customers found.\n");
        return;
    }

    printf("\n--- Returning Customers ---\n");
    printf("License Plate\tFirst Name\tLast Name\n");

    // Loop through the customers and display their information
    for (int i = 0; i < customerCount; i++) {
        printf("%s\t%s\t%s\n", customers[i].licensePlate, customers[i].firstName, customers[i].lastName);
    }

    printf("---------------------------\n");
}

// Display sales report
void displaySalesReport() {
    if (customerCount == 0) {
        printf("No sales data available.\n");
        return;
    }

    float totalSales = 0;
    int totalCustomers = customerCount;
    int cashPayments = 0;
    int cardPayments = 0;

    // Loop through the customers and accumulate total sales and payment methods
    for (int i = 0; i < customerCount; i++) {
        totalSales += customers[i].totalCost;
        if (customers[i].paymentMethod == 1) {
            cashPayments++;
        } else if (customers[i].paymentMethod == 2) {
            cardPayments++;
        }
    }

    // Display the report
    printf("\n--- Sales Report ---\n");
    printf("Total Customers: %d\n", totalCustomers);
    printf("Total Sales: $%.2f\n", totalSales);
    printf("Cash Payments: %d\n", cashPayments);
    printf("Card Payments: %d\n", cardPayments);
    printf("--------------------\n");
}


// Exit program
void exitProgram() {
    printf("Exiting the program...\n");
}
