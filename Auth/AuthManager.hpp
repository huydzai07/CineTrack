#pragma once
#include "../user/User.hpp"
#include "../user/Admin.hpp"
#include "../user/Customer.hpp"
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

class AuthManager {
private:
    // [DATA STRUCTURES & MEMORY] Using a vector of shared_ptr to safely manage User objects polymorphically
    std::vector<std::shared_ptr<User>> users;
    // [FILE I/O] Path to the CSV file storing persistent user credentials and data
    const std::string userFile = "data/users.csv";

public:
    // [FILE I/O] Load user data from CSV into RAM at system startup
    void loadUsersFromFile() {
        std::ifstream file(userFile);
        if (!file.is_open()) return;

        std::string line, id, name, pass, role, ageStr, ptsStr;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::stringstream ss(line);
            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, pass, ',');
            std::getline(ss, role, ',');
            std::getline(ss, ageStr, ','); // Extract Age column
            std::getline(ss, ptsStr, ','); // [UPGRADE] Extract Loyalty Points column

            int age = 0;
            int points = 0;
            if (!ageStr.empty()) age = std::stoi(ageStr);
            if (!ptsStr.empty()) points = std::stoi(ptsStr);

            // [OOP] Polymorphic instantiation based on the role retrieved from the database
            if (role == "Admin") {
                // Admin does not strictly require age or points
                users.push_back(std::make_shared<Admin>(id, name, pass)); 
            } else if (role == "Customer") {
                // Customer requires full demographic and loyalty data
                users.push_back(std::make_shared<Customer>(id, name, pass, age, points)); 
            }
        }
        file.close();
    }

    // [ALGORITHM] Linear search through the user vector for authentication
    std::shared_ptr<User> login() {
        std::string inputName, inputPass;
        std::cout << "\n-- LOGIN --\n";
        std::cout << "Enter username: ";
        std::cin >> inputName;
        std::cout << "Enter password: ";
        std::cin >> inputPass;

        
        std::cin.ignore(10000, '\n');

        for (const auto& user : users) {
            if (user->getUsername() == inputName && user->checkPassword(inputPass)) {
                std::cout << "=> Login successful with " << user->getRole() << " privileges!\n";
                system("pause"); // Dừng 1 nhịp trước khi vào Menu chính
                return user; 
            }
        }
        std::cout << "=> Error: Incorrect username or password!\n";
        system("pause"); // FIX CHỚP TẮT: Dừng màn hình để đọc lỗi đăng nhập
        return nullptr;
    }

    // [FILE I/O & DATA STRUCTURES] Register a new customer and persist their data
    void registerUser() {
        std::string inputName, inputPass;
        int inputAge = 0; 
        
        std::cout << "\n--- REGISTER NEW CUSTOMER ---\n";

        bool validName = false;
        while (!validName) {
            std::cout << "Enter new username (no spaces/commas, or '0' to cancel): ";
            std::cin >> inputName;
            std::cin.ignore(10000, '\n'); 

            if (inputName == "0") {
                std::cout << "=> Registration canceled.\n";
                system("pause");
                return;
            }

            // [CRITICAL FIX] Chặn người dùng nhập dấu phẩy phá hỏng file CSV
            if (inputName.find(',') != std::string::npos) {
                std::cout << "=> Error: Username cannot contain commas (,). Please try again.\n\n";
                continue;
            }

            bool exists = false;
            for (const auto& user : users) {
                if (user->getUsername() == inputName) {
                    exists = true; break;
                }
            }

            if (exists) {
                std::cout << "=> Error: Username already exists! Please try another.\n\n";
            } else {
                validName = true;
            }
        }


        while (true) {
            std::cout << "Enter new password (no spaces/commas): ";
            std::cin >> inputPass;
            std::cin.ignore(10000, '\n');

            if (inputPass.find(',') != std::string::npos) {
                std::cout << "=> Error: Password cannot contain commas (,). Please try again.\n";
            } else {
                break;
            }
        }

        while (true) {
            std::cout << "Enter your age: ";
            std::cin >> inputAge;

            if (std::cin.fail() || inputAge <= 0) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
                std::cout << "=> Error: Invalid age! Please enter a valid positive number.\n";
            } else {
                std::cin.ignore(10000, '\n'); 
                break; 
            }
        }

        std::string newId = "C" + std::to_string(users.size() + 1);
        users.push_back(std::make_shared<Customer>(newId, inputName, inputPass, inputAge, 0));

        std::ofstream file(userFile, std::ios::app);
        if (file.is_open()) {
            file << newId << "," << inputName << "," << inputPass << ",Customer," << inputAge << ",0\n";
            file.close();
            std::cout << "=> Registration successful! You can now login.\n";
            system("pause"); 
        } else {
            std::cout << "=> Error: Cannot write to " << userFile << "\n";
            system("pause");
        }
    }
    // [FILE I/O & UPGRADE] Dynamically update loyalty points directly in the persistent CSV file
    void updatePointsInFile(const std::string& targetId, int newPoints) {
        std::ifstream fileIn(userFile);
        std::vector<std::string> lines;
        std::string line, id, name, pass, role, ageStr, ptsStr;
        
        if (fileIn.is_open()) {
            while (std::getline(fileIn, line)) {
                if (line.empty()) continue;
                std::stringstream ss(line);
                std::getline(ss, id, ',');
                
                if (id == targetId) {
                    std::getline(ss, name, ','); std::getline(ss, pass, ',');
                    std::getline(ss, role, ','); std::getline(ss, ageStr, ',');
                    // Update the string line with the new points value
                    lines.push_back(id + "," + name + "," + pass + "," + role + "," + ageStr + "," + std::to_string(newPoints));
                } else {
                    lines.push_back(line); // Preserve other users' data without alteration
                }
            }
            fileIn.close();
            
            // Overwrite the entire file with updated data (trunc mode)
            std::ofstream fileOut(userFile, std::ios::trunc);
            for (const auto& l : lines) fileOut << l << "\n";
            fileOut.close();
        }
    }
};
