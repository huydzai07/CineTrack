#pragma once // Prevents multiple inclusions of the header file
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "../film/FilmCatalog.hpp"

class ReviewManager {
private:
    // [FILE I/O] Relative path to the persistent storage file for reviews
    const std::string reviewFile = "data/reviews.csv";
    
    void sanitizeCSV(std::string& input) {
        for (char& c : input) {
            if (c == ',') c = ' ';
        }
    }

public:
    // [OOP] Method passing FilmCatalog by reference to allow real-time state updates
    void leaveReview(const std::string& customerId, FilmCatalog& catalog) {
        std::cout << "\n-- WRITE A REVIEW --\n";
        catalog.displayWithFilter();
        
        std::string filmId, comment;
        int stars;
        
        // [UPDATE] Exit sequence at the Film ID input stage
        std::cout << "\nEnter the ID of the film you want to review (or '0' to cancel): ";
        std::cin >> filmId;
        
        if (filmId == "0") {
            std::cout << "=> Review operation canceled.\n";
            return;
        }
        
        if (catalog.getFilmTitleById(filmId) == "Unknown Film") {
            std::cout << "=> Error: Film does not exist!\n";
            return;
        }

        // [UPDATE] Exit sequence at the Star Rating input stage
        std::cout << "Enter a star rating (1-5) or '0' to cancel: ";
        std::cin >> stars;
        
        // [ERROR HANDLING] Input validation to prevent infinite loops if user types a character instead of an integer
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "=> Error: Please enter a valid integer!\n";
            return;
        }

        if (stars == 0) {
            std::cout << "=> Review operation canceled.\n";
            return;
        }
        if (stars < 1 || stars > 5) {
            std::cout << "=> Error: Star rating must be between 1 and 5!\n";
            return;
        }

        std::cin.ignore(); // Flush the buffer before using getline
        std::cout << "Write your comment (without diacritics): ";
        std::getline(std::cin, comment);
        sanitizeCSV(comment);

        // [ALGORITHM] Retrieve real-time system timestamp for the review
        std::time_t now = std::time(nullptr);

        // [FILE I/O] Append mode (std::ios::app) ensures existing records are not overwritten
        std::ofstream file(reviewFile, std::ios::app);
        if (file.is_open()) {
            file << customerId << "," << filmId << "," << stars << "," << comment << "," << now << "\n";
            file.close();
            
            // [ALGORITHM] Incrementally recalculate and update the running average rating of the film in RAM
            catalog.addReviewToFilm(filmId, stars);
            std::cout << "=> Review saved! The film's average rating has been dynamically updated.\n";
        } else {
            std::cout << "=> Error: Unable to open reviews.csv file.\n";
        }
    }
};
