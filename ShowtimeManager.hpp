#pragma once // Prevents multiple inclusions of the header file
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class ShowtimeManager {
private:
    // [FILE I/O] Path to the persistent storage file for cinema showtimes
    const std::string file = "data/showtimes.csv";

public:
    // [FILE I/O] Directly append a new showtime to the persistent storage on disk
    void addShowtime(const std::string& filmId, const std::string& time) {
        std::ofstream out(file, std::ios::app);
        if (out.is_open()) {
            out << filmId << "," << time << "\n";
            std::cout << "=> Successfully added showtime [" << time << "] for film [" << filmId << "]\n";
        } else {
            std::cout << "=> Error: Unable to open showtimes.csv file.\n";
        }
    }

    // [FILE I/O & ALGORITHM] Read file and parse CSV linearly to display all available showtimes for a specific film
    void displayShowtimesForFilm(const std::string& filmId) {
        std::ifstream in(file);
        if (!in.is_open()) return;

        std::string line, fId, time;
        bool found = false;
        std::cout << "Showtimes: ";
        
        while (std::getline(in, line)) {
            std::stringstream ss(line);
            std::getline(ss, fId, ',');
            std::getline(ss, time, ',');

            if (fId == filmId) {
                std::cout << "[" << time << "] ";
                found = true;
            }
        }
        
        if (!found) std::cout << "No showtimes available.";
        std::cout << "\n";
    }

    // [ALGORITHM] Validate if the customer's inputted time exactly matches an existing showtime schedule
    bool isValidShowtime(const std::string& filmId, const std::string& targetTime) {
        std::ifstream in(file);
        if (!in.is_open()) return false;
        
        std::string line, fId, time;
        while (std::getline(in, line)) {
            std::stringstream ss(line);
            std::getline(ss, fId, ',');
            std::getline(ss, time, ',');
            if (fId == filmId && time == targetTime) return true;
        }
        return false;
    }
};