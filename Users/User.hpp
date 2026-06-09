#pragma once // Prevents multiple inclusions of the header file
#include <string>
#include <iostream>

class User {
protected:
    std::string userID;
    std::string username;
    std::string passwordHash;
    int age; 
    int points; // Member variable to store loyalty points

public:
    // Constructor: Added 'a' for age and 'p' for points (defaults to 0)
    User(std::string id, std::string name, std::string pass, int a = 0, int p = 0) 
        : userID(id), username(name), passwordHash(pass), age(a), points(p) {}

    // [MEMORY] Virtual Destructor (Crucial for Polymorphism and dynamic memory allocation)
    virtual ~User() = default;

    // [OOP] Pure virtual functions forcing derived classes to override them
    virtual void displayMenu() = 0; 
    virtual std::string getRole() const = 0;

    // Basic Getters/Setters
    std::string getUserID() const { return userID; }
    std::string getUsername() const { return username; }
    int getAge() const { return age; } 
    int getPoints() const { return points; } // [UPGRADE] Getter for loyalty points
    void setPoints(int p) { points = p; }    // [UPGRADE] Setter to update loyalty points
    
    bool checkPassword(const std::string& inputPass) const { return passwordHash == inputPass; }
};
