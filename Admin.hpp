#pragma once
#include "User.hpp"

class Admin : public User {
public:
    // [OOP] Constructor: Inherits from User. Age and points automatically default to 0 for Admin accounts.
    Admin(std::string id, std::string name, std::string pass)
        : User(id, name, pass) {}

    // [OOP] Overriding pure virtual function to return specific role
    std::string getRole() const override {
        return "Admin";
    }

    // [OOP] Overriding pure virtual function to display specific menu
    void displayMenu() override {
        std::cout << "\n--- ADMIN MENU ---\n";
        std::cout << "1. Add new film\n";
        std::cout << "2. Manage showtimes (View showtimes)\n"; // [UPDATE] Renamed for clarity
        std::cout << "3. Update Film Rating\n";
        std::cout << "4. Edit film details\n";
        std::cout << "5. Delete film\n";
        std::cout << "6. View Revenue Report\n"; // [UPDATE] Added revenue report generation
        std::cout << "0. Logout\n";
    }
};