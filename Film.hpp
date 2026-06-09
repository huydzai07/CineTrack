#pragma once // Prevents multiple inclusions of the header file
#include <string>
#include <iostream>

class Film {
private:
    // [OOP] Encapsulation: Restricting direct access to object attributes to protect internal state
    std::string id;
    std::string title;
    std::string genre;
    int year;
    double avgRating;
    int reviewCount = 0; // State variable tracking the total number of submitted reviews

public:
    // [OOP] Constructor: Initializes the film entity with a default average rating of 0.0
    Film(std::string f_id, std::string f_title, std::string f_genre, int f_year)
        : id(f_id), title(f_title), genre(f_genre), year(f_year), avgRating(0.0) {}

    // [UI/UX] Utility method to output formatted film details to the console
    void displayInfo() const {
        std::cout << "[" << id << "] " << title << " (" << year << ") - " 
                  << genre << " | Rating: " << avgRating << "*\n";
    }

    // [OOP] Getter methods providing secure, read-only access to private attributes
    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getGenre() const { return genre; }
    int getYear() const { return year; }
    double getAvgRating() const { return avgRating; }
    
    // [OOP] Setter methods allowing authorized modification of film details
    void setRating(double newRating) { avgRating = newRating; }
    void setTitle(const std::string& newTitle) { title = newTitle; }
    void setGenre(const std::string& newGenre) { genre = newGenre; }
    void setYear(int newYear) { year = newYear; }
    
    // [ALGORITHM] Incremental Rating Aggregation
    // Formula: newAvg = (oldAvg * count + newRating) / (count + 1)
    // Dynamically recalculates the film's overall rating in O(1) time complexity without storing historical data
    void addCustomerRating(double newStar) {
        avgRating = (avgRating * reviewCount + newStar) / (reviewCount + 1);
        reviewCount++;
    }
};