#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "SeatMap.hpp"
#include "../film/FilmCatalog.hpp"

class BookingManager {
private:
    // [FILE I/O] Persistent storage path for all booking transactions
    const std::string bookingFile = "data/bookings.csv";

public:
    // [FILE I/O] Load seat map state from disk (Reading the 6th column to avoid parsing alignment errors)
    void loadBookings(const std::string& filmId, const std::string& timeStr, SeatMap& map) {
        std::ifstream file(bookingFile);
        if (!file.is_open()) return;

        std::string line, uId, fId, tStr, rowStr, colStr, priceStr;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::getline(ss, uId, ',');
            std::getline(ss, fId, ',');
            std::getline(ss, tStr, ',');
            std::getline(ss, rowStr, ',');
            std::getline(ss, colStr, ',');
            std::getline(ss, priceStr, ','); // Extract price column

            if (fId == filmId && tStr == timeStr && !rowStr.empty() && !colStr.empty()) {
                map.markSeatAsBooked(rowStr[0], std::stoi(colStr));
            }
        }
        file.close();
    }


    // [FILE I/O & UI] Retrieve and display the user's booking history
    void displayBookingHistory(const std::string& currentUserId, const FilmCatalog& catalog) {
        std::ifstream file(bookingFile);
        if (!file.is_open()) return;

        std::cout << "\n--- BOOKING HISTORY ---\n";
        bool hasHistory = false;
        std::string line, uId, fId, tStr, rowStr, colStr, priceStr, codeStr;

        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::getline(ss, uId, ',');
            std::getline(ss, fId, ',');
            std::getline(ss, tStr, ',');
            std::getline(ss, rowStr, ',');
            std::getline(ss, colStr, ',');
            std::getline(ss, priceStr, ','); // Extract price
            std::getline(ss, codeStr, ',');  // Extract Booking Code

            if (uId == currentUserId && !rowStr.empty()) {
                std::string title = catalog.getFilmTitleById(fId);
                std::cout << "- Film: " << title << " [" << fId << "] | Time: " << tStr 
                          << " | Seat: " << rowStr << colStr;
                if (!priceStr.empty()) {
                    std::cout.precision(0);
                    std::cout << " | Price: " << std::fixed << std::stod(priceStr) << " VND";
                }
                if (!codeStr.empty()) std::cout << " | CODE: [" << codeStr << "]";
                std::cout << "\n";
                hasHistory = true;
            }
        }
        file.close();
        if (!hasHistory) std::cout << "=> You haven't booked any tickets yet!\n";
    }

    // [ALGORITHM & ERROR HANDLING] Generate revenue report by accumulating financial data with Try-Catch protection
    void displayRevenueReport() {
        std::cout << "\n--- REVENUE REPORT ---\n";
        try {
            std::ifstream file(bookingFile);
            if (!file.is_open()) {
                throw std::runtime_error("Unable to open bookings.csv file");
            }

            int totalTickets = 0;
            double totalRevenue = 0.0;
            std::string line, uId, fId, tStr, rowStr, colStr, priceStr;

            while (std::getline(file, line)) {
                if (line.empty()) continue;
                std::stringstream ss(line);
                std::getline(ss, uId, ',');
                std::getline(ss, fId, ',');
                std::getline(ss, tStr, ',');
                std::getline(ss, rowStr, ',');
                std::getline(ss, colStr, ',');
                std::getline(ss, priceStr, ','); // Extract ticket price

                if (!priceStr.empty()) {
                    totalRevenue += std::stod(priceStr); // Convert string to double and accumulate
                    totalTickets++;
                }
            }
            file.close();

            std::cout << "Total tickets sold: " << totalTickets << " tickets\n";
            std::cout << "---------------------------------\n";
            std::cout.precision(0); 
            std::cout << "TOTAL REVENUE     : " << std::fixed << totalRevenue << " VND\n";

        } catch (const std::exception& e) {
            std::cout << "=> System error: " << e.what() << "\n";
            std::cout << "=> The system remains stable. Please try again later.\n";
        }
    }

    // [FILE I/O] Persist booking transaction to disk including dynamic price, Booking Code, and Points Used
    void saveBooking(const std::string& userId, const std::string& filmId, const std::string& timeStr, char row, int col, double price, const std::string& bookingCode, int pointsUsed) {
        std::ofstream file(bookingFile, std::ios::app);
        if (file.is_open()) {
            file << userId << "," << filmId << "," << timeStr << "," << (char)toupper(row) << "," << col << "," << price << "," << bookingCode << "," << pointsUsed << "\n";
            file.close();
        }
    }

    // [DATA STRUCTURES] Ticket cancellation with Point Refund Logic
    int cancelBooking(const std::string& currentUserId, const FilmCatalog& catalog) {
        std::ifstream file(bookingFile);
        if (!file.is_open()) {
            std::cout << "=> The system currently has no booking data!\n";
            return 0;
        }

        std::vector<std::string> allLines;
        std::vector<int> userTicketIndices;
        std::vector<double> userTicketPrices; 
        std::vector<int> userTicketPointsUsed; // Track points used for refund
        
        std::string line, uId, fId, tStr, rowStr, colStr, priceStr, codeStr, ptsUsedStr;
        int displayCount = 0;

        std::cout << "\n--- CANCEL BOOKING ---\n";
        while (std::getline(file, line)) {
            allLines.push_back(line);
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::getline(ss, uId, ',');
            
            if (uId == currentUserId) {
                displayCount++;
                userTicketIndices.push_back(allLines.size() - 1); 
                
                std::getline(ss, fId, ','); std::getline(ss, tStr, ',');
                std::getline(ss, rowStr, ','); std::getline(ss, colStr, ',');
                std::getline(ss, priceStr, ','); std::getline(ss, codeStr, ',');
                std::getline(ss, ptsUsedStr, ','); 

                double price = 0; int ptsUsed = 0;
                if (!priceStr.empty()) price = std::stod(priceStr);
                if (!ptsUsedStr.empty()) ptsUsed = std::stoi(ptsUsedStr);
                
                userTicketPrices.push_back(price);
                userTicketPointsUsed.push_back(ptsUsed);

                std::string title = catalog.getFilmTitleById(fId);
                std::cout << "[" << displayCount << "] Film: " << title << " | Showtime: " << tStr 
                          << " | Seat: " << rowStr << colStr << " | Price: " << priceStr << " VND";
                if (!codeStr.empty()) std::cout << " | CODE: " << codeStr;
                std::cout << "\n";
            }
        }
        file.close();

        if (displayCount == 0) {
            std::cout << "=> You have no bookings to cancel!\n";
            return 0;
        }

        std::vector<int> linesToRemove;
        double netPointsChange = 0; // Change in actual points (Refund - Deduction)

        while (true) {
            int choice;
            std::cout << "\nEnter ticket number to cancel (or '0' to finish/exit): ";
            std::cin >> choice;

            if (std::cin.fail()) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
                std::cout << "=> Error: Invalid input. Please enter a valid number.\n";
                continue;
            }

            if (choice == 0) break;

            if (choice > 0 && choice <= displayCount) {
                int targetIndex = userTicketIndices[choice - 1];
                
                bool alreadySelected = false;
                for (int idx : linesToRemove) {
                    if (idx == targetIndex) { alreadySelected = true; break; }
                }

                if (alreadySelected) {
                    std::cout << "=> Error: Ticket [" << choice << "] is already selected for cancellation.\n";
                } else {
                    linesToRemove.push_back(targetIndex);
                    
                    // Point Calculation Logic: (Points used to purchase tickets) - (Points added from the payment for those tickets)
                    int pointsEarnedToDeduct = (int)(userTicketPrices[choice - 1] / 1000.0);
                    int pointsToRefund = userTicketPointsUsed[choice - 1];
                    netPointsChange += (pointsToRefund - pointsEarnedToDeduct);
                    
                    std::cout << "=> Ticket [" << choice << "] marked for cancellation.\n";
                }
            } else {
                std::cout << "=> Invalid ticket number!\n";
            }
        }

        if (!linesToRemove.empty()) {
            std::sort(linesToRemove.begin(), linesToRemove.end(), std::greater<int>());

            for (int idx : linesToRemove) { 
                allLines.erase(allLines.begin() + idx); 
            }
        

            for (int idx : linesToRemove) { allLines.erase(allLines.begin() + idx); }

            std::ofstream outFile(bookingFile, std::ios::trunc); 
            for (const auto& l : allLines) { outFile << l << "\n"; }
            outFile.close();

            std::cout << "\n=> Successfully canceled " << linesToRemove.size() << " ticket(s)!\n";
            return (int)netPointsChange; // Returns the number of points to be added/subtracted from the account
        }
        return 0; 
    }
};
