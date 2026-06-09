#pragma once
#include <iostream>
#include <vector>
#include <string>

class SeatMap {
private:
    // [DATA STRUCTURES] 2D vector representing the spatial seating arrangement. 'O' = Available, 'X' = Booked
    std::vector<std::vector<char>> seats;
    std::vector<char> rowLabels = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L'};

public:
    SeatMap() {
        // [ALGORITHM] Initialize the cinema hall with a total capacity of 152 seats
        for (int i = 0; i < 11; ++i) {
            if (i == 10) { 
                // Row L (index 10) acts as the Deluxe tier with 12 seats
                seats.push_back(std::vector<char>(12, 'O'));
            } else { 
                // Rows A through K contain 14 seats each
                seats.push_back(std::vector<char>(14, 'O'));
            }
        }
    }

    // [UI/UX] Console-based visualizer for the spatial seat mapping
    void displayMap() const {
        std::cout << "\n========================= CINEMA SCREEN =========================\n\n";
        
        // Print column indices (1 to 14) at the top header
        std::cout << "    ";
        for (int i = 1; i <= 14; ++i) {
            if (i < 10) std::cout << i << "   ";
            else std::cout << i << "  ";
        }
        std::cout << "\n";

        // Iterate through the rows to render the seating layout
        for (size_t i = 0; i < seats.size(); ++i) {
            std::cout << rowLabels[i] << "  ";

            // Center-align row L (which only has 12 seats) for visual symmetry
            if (i == 10) std::cout << "    "; 

            for (size_t j = 0; j < seats[i].size(); ++j) {
                std::cout << "[" << seats[i][j] << "] ";
            }
            std::cout << "\n";
        }
        
        std::cout << "\n----------------------------------------------------------------------\n";
        std::cout << "Row A-D: Standard | Row E-K: VIP | Row L: Deluxe\n";
        std::cout << "[O]: Available    | [X]: Booked\n";
    }

    int getAvailableSeats() const {
        int count = 0;
        for (size_t i = 0; i < seats.size(); ++i) {
            for (size_t j = 0; j < seats[i].size(); ++j) {
                if (seats[i][j] == 'O') {
                    count++;
                }
            }
        }
        return count;
    }

    // [ALGORITHM] Validates constraints and reserves a requested seat dynamically
    bool bookSeat(char row, int col) {
        int rIndex = -1;
        // Map the character row label (e.g., 'A') to its corresponding vector index
        for (size_t i = 0; i < rowLabels.size(); ++i) {
            if (rowLabels[i] == toupper(row)) {
                rIndex = i;
                break;
            }
        }

        if (rIndex == -1) {
            std::cout << "=> Error: This row does not exist!\n";
            return false;
        }

        // Convert 1-based column input to 0-based vector index
        int cIndex = col - 1;
        if (cIndex < 0 || cIndex >= seats[rIndex].size()) {
            std::cout << "=> Error: This seat number does not exist!\n";
            return false;
        }

        if (seats[rIndex][cIndex] == 'X') {
            std::cout << "=> Error: This seat is already booked!\n";
            return false;
        }

        // Mutate the state to mark the seat as booked
        seats[rIndex][cIndex] = 'X';
        std::cout << "=> Successfully booked seat " << (char)toupper(row) << col << "!\n";
        return true;
    }

    // [FILE I/O & MEMORY] Silently mutates the RAM state during startup to reflect persistent CSV bookings
    void markSeatAsBooked(char row, int col) {
        int rIndex = -1;
        for (size_t i = 0; i < rowLabels.size(); ++i) {
            if (rowLabels[i] == toupper(row)) { 
                rIndex = i; 
                break; 
            }
        }
        if (rIndex != -1) {
            int cIndex = col - 1;
            if (cIndex >= 0 && cIndex < seats[rIndex].size()) {
                seats[rIndex][cIndex] = 'X';
            }
        }
    }
};
