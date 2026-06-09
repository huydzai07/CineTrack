#pragma once
#include "User.hpp"

class Customer : public User {
public:
    // [UPDATE] Pass id, name, password, age (a), and points (p) to the base User class
    Customer(std::string id, std::string name, std::string pass, int a, int p = 0) 
        : User(id, name, pass, a, p) {}

    // [OOP] Overriding pure virtual function to return specific role
    std::string getRole() const override {
        return "Customer";
    }

    // [OOP] Overriding pure virtual function to display specific menu
    void displayMenu() override {
        std::cout << "\n--- CUSTOMER MENU ---\n";
        std::cout << "1. Search films\n";
        std::cout << "2. Book tickets\n";
        std::cout << "3. Booking history\n";
        std::cout << "4. Write a review\n";
        std::cout << "5. Cancel a booking\n";
        std::cout << "6. View Membership Profile (Points)\n"; // TÍNH NĂNG MỚI
        std::cout << "0. Logout\n";
    }
};
