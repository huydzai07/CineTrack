#pragma once
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Film.hpp"

class FilmCatalog {
private:
    // [DATA STRUCTURES & MEMORY] Using a vector of unique_ptr to automatically manage memory and prevent leaks
    std::vector<std::unique_ptr<Film>> catalog;
    // [FILE I/O] Path to the CSV file storing persistent film records
    const std::string filmFile = "data/films.csv"; 

public:
    void saveFilmsToFile() const {
        std::ofstream file(filmFile);
        for (const auto& film : catalog) {
            file << film->getId() << "," << film->getTitle() << "," 
                 << film->getGenre() << "," << film->getYear() << "," 
                 << film->getAvgRating() << "\n"; // Persist the average rating
        }
        file.close();
    }

    void loadFilmsFromFile() {
        std::ifstream file(filmFile);
        if (!file.is_open()) return;

        std::string line, id, title, genre, yearStr, ratingStr;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::getline(ss, id, ',');
            std::getline(ss, title, ',');
            std::getline(ss, genre, ',');
            std::getline(ss, yearStr, ',');
            std::getline(ss, ratingStr, ','); // Read the average rating field

            if (!id.empty()) {
                int year = std::stoi(yearStr);
                auto newFilm = std::make_unique<Film>(id, title, genre, year);
                if (!ratingStr.empty()) {
                    // [ALGORITHM] Type casting string to double for rating aggregation
                    newFilm->setRating(std::stod(ratingStr)); 
                }
                catalog.push_back(std::move(newFilm));
            }
        }
        file.close();
    }

    void addFilm(std::string id, std::string title, std::string genre, int year) {
        catalog.push_back(std::make_unique<Film>(id, title, genre, year));
        std::cout << "=> Successfully added: " << title << "\n";
    }

    // [OOP] Method to modify existing film attributes
    void editFilm(const std::string& searchId, const std::string& newTitle, const std::string& newGenre, int newYear) {
        for (auto& film : catalog) {
            if (film->getId() == searchId) {
                film->setTitle(newTitle);
                film->setGenre(newGenre);
                film->setYear(newYear);
                std::cout << "=> Successfully updated details for film [" << searchId << "]!\n";
                return;
            }
        }
        std::cout << "=> Error: Film ID not found for editing!\n";
    }

    // [MEMORY & ALGORITHM] Safely delete a film using the Erase-Remove idiom with std::remove_if and lambda expressions
    void deleteFilm(const std::string& searchId) {
        auto it = std::remove_if(catalog.begin(), catalog.end(),
            [&searchId](const std::unique_ptr<Film>& film) {
                return film->getId() == searchId;
            });

        if (it != catalog.end()) {
            catalog.erase(it, catalog.end()); // Safely deallocate memory from the unique_ptr vector
            std::cout << "=> Successfully deleted film [" << searchId << "]!\n";
        } else {
            std::cout << "=> Error: Film ID not found for deletion!\n";
        }
    }

    // [FILE I/O & UI] Helper method to dynamically read the showtimes file and print available slots under the film name
    void printShowtimes(const std::string& filmId) const {
        std::ifstream stFile("data/showtimes.csv");
        std::string stLine, sFid, sTime;
        bool hasShowtime = false;
        
        std::cout << "       => Showtimes: ";
        if (stFile.is_open()) {
            while(std::getline(stFile, stLine)) {
                if(stLine.empty()) continue;
                std::stringstream ss(stLine);
                std::getline(ss, sFid, ',');
                std::getline(ss, sTime, ',');
                if (sFid == filmId && !sTime.empty()) {
                    std::cout << "[" << sTime << "] ";
                    hasShowtime = true;
                }
            }
            stFile.close();
        }
        if (!hasShowtime) std::cout << "No showtimes available";
        std::cout << "\n";
    }

    void displayAllFilms() const {
        if (catalog.empty()) {
            std::cout << "The catalog is currently empty.\n";
            return;
        }
        std::cout << "\n--- FILM CATALOG ---\n";
        for (const auto& film : catalog) {
            film->displayInfo();
            printShowtimes(film->getId()); // Call helper to print associated showtimes
        }
    }

    // [UI/UX] Display a compact filter menu before requiring a Film ID input
    void displayWithFilter() const {
        std::string choice;
        std::cout << "\n[1] View all films | [2] Quick search by keyword | [0] Skip\n";
        std::cout << "Select display mode: ";
        std::cin >> choice;

        if (choice == "1") {
            displayAllFilms(); // Print all films along with their showtimes
        } else if (choice == "2") {
            std::string keyword;
            std::cout << "Enter keyword (Title or Genre): ";
            std::cin.ignore();
            std::getline(std::cin, keyword);
            
            std::cout << "\n--- FILTER RESULTS ---\n";
            bool found = false;
            for (const auto& f : catalog) {
                if (f->getTitle().find(keyword) != std::string::npos ||
                    f->getGenre().find(keyword) != std::string::npos) {
                    std::cout << "[" << f->getId() << "] " << f->getTitle() << " (" << f->getYear() << ")\n";
                    printShowtimes(f->getId());
                    found = true;
                }
            }
            if (!found) std::cout << "=> No matching films found!\n";
        }
    }

    // [ALGORITHM] Advanced search engine featuring multi-criteria linear filtering and lambda-based sorting
    void advancedSearch() const {
        std::cout << "\n-- ADVANCED SEARCH FILM --\n";
        std::cout << "1. View all films\n";
        std::cout << "2. Search films (by Title or Genre)\n";
        std::cout << "0. Cancel\n";
        std::cout << "Select option: ";
        std::string opt;
        std::cin >> opt;

        if (opt == "0") {
            std::cout << "=> Operation canceled.\n";
            return;
        }

        std::vector<Film*> results;
        
        if (opt == "1") { // Retrieve all films
            for (const auto& film : catalog) {
                results.push_back(film.get());
            }
        } else if (opt == "2") { // Apply linear filter
            std::string keyword;
            std::cout << "Enter search keyword: ";
            std::cin.ignore();
            std::getline(std::cin, keyword);
            
            for (const auto& film : catalog) {
                if (film->getTitle().find(keyword) != std::string::npos ||
                    film->getGenre().find(keyword) != std::string::npos) {
                    results.push_back(film.get());
                }
            }
        } else {
            std::cout << "=> Invalid option.\n";
            return;
        }

        if (results.empty()) {
            std::cout << "=> No matching films found!\n";
            return;
        }

        // --- SORTING ENGINE ---
        std::string sortChoice;
        std::cout << "\nChoose sorting criteria:\n";
        std::cout << "1. By Rating (Descending)\n";
        std::cout << "2. By Release Year (Newest)\n";
        std::cout << "3. By Title (A-Z)\n";
        std::cout << "0. Skip sorting (Default view)\n";
        std::cout << "Your choice: ";
        std::cin >> sortChoice;

        if (sortChoice == "1") {
            std::sort(results.begin(), results.end(), [](Film* a, Film* b) { return a->getAvgRating() > b->getAvgRating(); });
        } else if (sortChoice == "2") {
            std::sort(results.begin(), results.end(), [](Film* a, Film* b) { return a->getYear() > b->getYear(); });
        } else if (sortChoice == "3") {
            std::sort(results.begin(), results.end(), [](Film* a, Film* b) { return a->getTitle() < b->getTitle(); });
        }

        // 3. Print processed results (WITH SHOWTIMES)
        std::cout << "\n--- SEARCH RESULTS ---\n";
        for (const auto& f : results) {
            std::cout << "[" << f->getId() << "] " << f->getTitle() 
                      << " (" << f->getYear() << ") - " << f->getGenre() 
                      << " | Rating: " << f->getAvgRating() << "*\n";
            
            // [FILE I/O] Read showtimes.csv directly to map and append showtimes to search results
            std::ifstream stFile("data/showtimes.csv");
            std::string stLine, sFid, sTime;
            bool hasShowtime = false;
            
            std::cout << "       => Showtimes: ";
            if (stFile.is_open()) {
                while(std::getline(stFile, stLine)) {
                    if(stLine.empty()) continue;
                    std::stringstream ss(stLine);
                    std::getline(ss, sFid, ',');
                    std::getline(ss, sTime, ',');
                    
                    if (sFid == f->getId() && !sTime.empty()) {
                        std::cout << "[" << sTime << "] ";
                        hasShowtime = true;
                    }
                }
                stFile.close();
            }
            
            if (!hasShowtime) {
                std::cout << "No showtimes available";
            }
            std::cout << "\n";
        }
    }

    // Helper method to retrieve a film's title by its ID
    std::string getFilmTitleById(const std::string& searchId) const {
        for (const auto& film : catalog) {
            if (film->getId() == searchId) return film->getTitle();
        }
        return "Unknown Film";
    }

    void updateRating(const std::string& id, double newRating) {
        for (const auto& film : catalog) {
            if (film->getId() == id) {
                film->setRating(newRating);
                std::cout << "=> Successfully updated Rating for '" << film->getTitle() << "' to " << newRating << "*!\n";
                return;
            }
        }
        std::cout << "=> Error: Film with this ID not found!\n";
    }

    void addReviewToFilm(const std::string& id, double star) {
        for (const auto& film : catalog) {
            if (film->getId() == id) {
                film->addCustomerRating(star);
                return;
            }
        }
    }
};
