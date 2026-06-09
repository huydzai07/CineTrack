#include <iostream>
#include <memory>
#include <string>
#include "auth/AuthManager.hpp"
#include "film/FilmCatalog.hpp"
#include "booking/SeatMap.hpp"
#include "booking/BookingManager.hpp"
#include "booking/ShowtimeManager.hpp"
#include "review/ReviewManager.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdlib>

void displayMainMenu() {
    std::cout << "\n=== CINETRACK SYSTEM ===\n";
    std::cout << "1. Login\n";
    std::cout << "2. Register\n";
    std::cout << "0. Exit\n";
    std::cout << "Select option: ";
}

int main() {
    srand(time(0));
    // [FILE I/O] Initialize system managers and preload data from CSV files into RAM
    AuthManager auth;
    auth.loadUsersFromFile();
    
    FilmCatalog catalog; 
    catalog.loadFilmsFromFile(); 

    int choice = -1;
    while (choice != 0) {
        system("cls"); // Clears the console for a minimalist UI experience
        displayMainMenu();
        std::cin >> choice;
        
        // [ERROR HANDLING] Buffer clearing mechanism. 
        // Prevents infinite loops and system crashes if the user inputs a char/string instead of an int.
        if (std::cin.fail()) {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(10000, '\n'); // Discard invalid characters in the input buffer
            std::cout << "=> Error: Please enter an integer only!\n";
            choice = -1; // Reset variable to safely continue the loop
            continue; 
        }

        switch (choice) {
            case 1: { 
                std::cout << "\n--- LOGIN ---\n";
                // [OOP & MEMORY] Polymorphism applied here. 
                // Using std::shared_ptr (Smart Pointer) to securely manage dynamic memory 
                // of the abstract User class, preventing memory leaks automatically.
                std::shared_ptr<User> loggedInUser = auth.login(); 
                
                if (loggedInUser != nullptr) {
                    int subChoice = -1;
                    while (subChoice != 0) {
                        // [OOP] Dynamic binding: calls the overridden displayMenu() of either Admin or Customer
                        loggedInUser->displayMenu();
                        std::cout << "Select option: ";
                        std::cin >> subChoice;

                        // [ERROR HANDLING] Input validation for sub-menus
                        if (std::cin.fail()) {
                            std::cin.clear(); 
                            std::cin.ignore(10000, '\n'); 
                            std::cout << "=> Error: Please enter an integer only!\n";
                            subChoice = -1; 
                            continue; 
                        }

                        if (subChoice == 0) {
                            std::cout << "=> Logging out...\n";
                            break; 
                        }

                        // ==========================================
                        // --- ADMIN ROLE HANDLING ---
                        // ==========================================
                        if (loggedInUser->getRole() == "Admin") {
                            if (subChoice == 1) { 
                                std::string id, title, genre;
                                int year;
                                std::cout << "\n-- ADD NEW FILM --\n";
                                std::cout << "Enter Film ID (e.g., f001) or '0' to cancel: ";
                                std::cin >> id;
                                if (id == "0") { std::cout << "=> Operation canceled.\n"; continue; }
                                
                                std::cin.ignore(); 
                                std::cout << "Enter Title: ";
                                std::getline(std::cin, title);
                                std::cout << "Enter Genre: ";
                                std::getline(std::cin, genre);
                                std::cout << "Enter Year: ";
                                std::cin >> year;
                                
                                catalog.addFilm(id, title, genre, year);
                                catalog.displayAllFilms();
                            } 
                            else if (subChoice == 2) { 
                                std::cout << "\n-- MANAGE SHOWTIMES --\n";
                                catalog.displayWithFilter(); 
                                
                                std::string targetId;
                                std::cout << "\nEnter Film ID to view/add showtimes (or '0' to cancel): ";
                                std::cin >> targetId;
                                if (targetId == "0") continue;

                                ShowtimeManager showtimeMgr;
                                std::cout << "\n=> Current showtimes:\n";
                                showtimeMgr.displayShowtimesForFilm(targetId);
                                
                                std::string timeStr;
                                // Loop to allow adding multiple showtimes continuously without returning to main menu
                                while (true) {
                                    std::cout << "\nEnter showtime to add (e.g., 18:30) or '0' to finish: ";
                                    std::cin >> timeStr;
                                    
                                    if (timeStr == "0") {
                                        std::cout << "=> Showtime update completed!\n";
                                        break; 
                                    }
                                    
                                    showtimeMgr.addShowtime(targetId, timeStr);
                                }
                            }
                            else if (subChoice == 3) { 
                                std::string targetId;
                                double newRate;
                                std::cout << "\n-- UPDATE RATING --\n";
                                std::cout << "Enter Film ID to update or '0' to cancel: ";
                                std::cin >> targetId;
                                if (targetId == "0") { std::cout << "=> Operation canceled.\n"; continue; }
                                
                                std::cout << "Enter new rating (0.0 - 5.0): ";
                                std::cin >> newRate;
                                
                                catalog.updateRating(targetId, newRate);
                            }
                            else if (subChoice == 4) { 
                                std::string searchId, newTitle, newGenre;
                                int newYear;
                                std::cout << "\n-- EDIT FILM --\n";
                                catalog.displayWithFilter();
                                
                                std::cout << "\nEnter Film ID to edit or '0' to cancel: ";
                                std::cin >> searchId;
                                if (searchId == "0") { std::cout << "=> Operation canceled.\n"; continue; }
                                std::cin.ignore(); 
                                
                                std::cout << "Enter new title: ";
                                std::getline(std::cin, newTitle);
                                std::cout << "Enter new genre: ";
                                std::getline(std::cin, newGenre);
                                std::cout << "Enter new year: ";
                                std::cin >> newYear;
                                
                                catalog.editFilm(searchId, newTitle, newGenre, newYear);
                            }
                            else if (subChoice == 5) { 
                                std::string searchId;
                                std::cout << "\n-- DELETE FILM --\n";
                                catalog.displayWithFilter();
                                
                                std::cout << "\nEnter Film ID to permanently delete or '0' to cancel: ";
                                std::cin >> searchId;
                                if (searchId == "0") { std::cout << "=> Operation canceled.\n"; continue; }
                                
                                // [MEMORY] Deleting object safely from the vector of unique_ptrs
                                catalog.deleteFilm(searchId);
                            }
                            else if (subChoice == 6) { 
                                BookingManager bookingMgr;
                                bookingMgr.displayRevenueReport();
                            }
                        }
                        
                        // ==========================================
                        // --- CUSTOMER ROLE HANDLING ---
                        // ==========================================
                        else if (loggedInUser->getRole() == "Customer") {
                            if (subChoice == 1) { 
                                std::cin.ignore(); 
                                // [ALGORITHM] Linear search & sorting algorithm execution
                                catalog.advancedSearch(); 
                            }
                            else if (subChoice == 2) { 
                                std::cout << "\n-- BOOK TICKETS --\n";
                                catalog.displayWithFilter();
                                
                                std::string selectedFilmId;
                                std::cout << "\nEnter Film ID you want to watch or '0' to cancel: ";
                                std::cin >> selectedFilmId;
                                if (selectedFilmId == "0") { std::cout << "=> Operation canceled.\n"; continue; }

                                std::string filmTitle = catalog.getFilmTitleById(selectedFilmId);
                                if (filmTitle == "Unknown Film") {
                                    std::cout << "=> Error: Film ID does not exist in the system!\n";
                                    continue; 
                                }

                                ShowtimeManager showtimeMgr;
                                std::cout << "\n=> SHOWTIMES FOR FILM [" << filmTitle << "] <=\n";
                                showtimeMgr.displayShowtimesForFilm(selectedFilmId);
                                
                                std::string selectedTime;
                                bool validTime = false;
                                while (!validTime) {
                                    std::cout << "Enter showtime you want to book (or '0' to cancel): ";
                                    std::cin >> selectedTime;
                                    if (selectedTime == "0") {
                                        std::cout << "=> Operation canceled.\n";
                                        break; 
                                    }
                                    if (showtimeMgr.isValidShowtime(selectedFilmId, selectedTime)) {
                                        validTime = true; 
                                    } else {
                                        std::cout << "=> Error: Invalid showtime! Please try again.\n";
                                    }
                                }
                                if (!validTime) continue; 

                                // [FILE I/O & ALGORITHM] Load 2D Seat Map representation from bookings.csv
                                SeatMap map;
                                BookingManager bookingMgr;
                                std::cout << "\n=> Loading seat map for film [" << filmTitle << "] - Showtime [" << selectedTime << "]...\n";
                                
                                bookingMgr.loadBookings(selectedFilmId, selectedTime, map);
                                map.displayMap();

                                int numTickets = 0;
                                // Correct number of ticket to pass
                                while (true) {
                                    std::cout << "\nEnter number of tickets to buy (or '0' to cancel): ";
                                    std::cin >> numTickets;
                                    
                                    if (std::cin.fail() || numTickets < 0) {
                                        std::cin.clear(); 
                                        std::cin.ignore(10000, '\n'); 
                                        std::cout << "=> Error: Number of tickets must be a valid integer!\n";
                                    } else {
                                        break;
                                    }
                                }
                                if (numTickets == 0) {
                                    std::cout << "=> Ticket booking canceled.\n";
                                    continue; 
                                }

                                // [ALGORITHM] Dynamic Pricing Module
                                // Base price calculated based on user demographic (Age)
                                int userAge = loggedInUser->getAge();
                                double basePrice = 0.0;
                                if (userAge < 22) basePrice = 70000.0;
                                else if (userAge >= 22 && userAge <= 70) basePrice = 120000.0;
                                else basePrice = 50000.0;

                                // [DATA STRUCTURES] STL Vectors acting as a temporary "Shopping Cart"
                                std::vector<char> cartRows;
                                std::vector<int> cartCols;
                                std::vector<double> cartPrices;
                                std::vector<std::string> cartNames;
                                double totalBatchPrice = 0.0;

                                // Loop to collect the required number of seats
                                for (int i = 1; i <= numTickets; i++) {
                                    char row;
                                    int col;
                                    bool seatValid = false;
                                    
                                    while (!seatValid) {
                                        std::cout << "\n[Ticket " << i << "/" << numTickets << "] Choose row (A-L) or '0' to cancel all: ";
                                        std::cin >> row;
                                        if (row == '0') {
                                            std::cout << "=> Ticket booking canceled.\n";
                                            break; 
                                        }
                                        
                                        std::cout << "[Ticket " << i << "/" << numTickets << "] Choose seat number (1-14): ";
                                        std::cin >> col;

                                        // Validates seat and temporarily marks it as [X] in RAM (SeatMap Object)
                                        if (map.bookSeat(row, col)) {
                                            seatValid = true;
                                            double surcharge = 0.0;
                                            std::string seatType = "";
                                            char upperRow = toupper(row);

                                            // [ALGORITHM] Spatial Zoning Price Surcharge
                                            if (upperRow >= 'A' && upperRow <= 'D') {
                                                seatType = "Regular"; surcharge = 0.0;
                                            } else if (upperRow >= 'E' && upperRow <= 'H') {
                                                seatType = "VIP"; surcharge = 80000.0;
                                            } else {
                                                seatType = "Deluxe"; surcharge = 130000.0;
                                            }

                                            double ticketPrice = basePrice + surcharge;
                                            totalBatchPrice += ticketPrice;

                                            // Add item properties to the vector cart
                                            cartRows.push_back(upperRow);
                                            cartCols.push_back(col);
                                            cartPrices.push_back(ticketPrice);
                                            cartNames.push_back(std::string(1, upperRow) + std::to_string(col) + " [" + seatType + "]");
                                        } else {
                                            std::cout << "=> Error: Seat does not exist or is already booked! Please choose again.\n";
                                        }
                                    }
                                    if (row == '0') {
                                        cartRows.clear(); // Clear cart to avoid partial booking if canceled midway
                                        break; 
                                    }
                                }

                                if (cartRows.empty()) continue; // Skip checkout if cart is empty

                                // --- CHECKOUT CONFIRMATION & LOYALTY SYSTEM ---
                                int currentPoints = loggedInUser->getPoints();
                                double discount = 0.0;
                                int pointsToUse = 0;

                                std::cout.precision(0);
                                std::cout << "\n--- CHECKOUT CONFIRMATION ---\n";
                                std::cout << "- Film : " << filmTitle << " | Showtime: " << selectedTime << "\n";
                                std::cout << "- Total tickets: " << numTickets << "\n";
                                std::cout << "- Selected seats:\n";
                                for (size_t i = 0; i < cartNames.size(); i++) {
                                    std::cout << "  + " << cartNames[i] << " : " << std::fixed << cartPrices[i] << " VND\n";
                                }
                                std::cout << "--------------------------------\n";
                                std::cout << "=> SUB-TOTAL : " << std::fixed << totalBatchPrice << " VND\n";

                                // 10 points= 1000 VND
                                if (currentPoints >= 10) {
                                    int maxDiscountVND = (currentPoints / 10) * 1000;
                                    std::cout << "=> You currently have " << currentPoints << " Membership Points.\n";
                                    std::cout << "=> (Rate: 10 Points = 1000 VND Discount. Max usable: " << maxDiscountVND << " VND).\n";
                                    
                                    while (true) {
                                        std::cout << "=> Enter points to redeem (or '0' to skip): ";
                                        std::cin >> pointsToUse;

                                        if (std::cin.fail() || pointsToUse < 0) {
                                            std::cin.clear(); std::cin.ignore(10000, '\n');
                                            std::cout << "=> Error: Invalid input. Please enter a valid number.\n";
                                        } else if (pointsToUse > currentPoints) {
                                            std::cout << "=> Error: You only have " << currentPoints << " points. Please enter a valid amount.\n";
                                        } else {
                                            break;
                                        }
                                    }
                                    // ------------------------------------------

                                    // rounded point for easy caculation
                                    pointsToUse = (pointsToUse / 10) * 10;
                                    discount = (pointsToUse / 10) * 1000.0;
                                    if (discount > totalBatchPrice) discount = totalBatchPrice; 
                                }

                                double finalPrice = totalBatchPrice - discount;
                                if (discount > 0) std::cout << "=> DISCOUNT  : -" << discount << " VND\n";
                                std::cout << "=> FINAL TOTAL: " << std::fixed << finalPrice << " VND\n";
                                std::cout << "=> Are you sure you want to pay? (Y/N): ";
                                
                                char confirm;
                                std::cin >> confirm;

                                if (toupper(confirm) == 'Y') { 
                                    // [ALGORITHM] GENERATE RANDOM BOOKING CODE
                                    std::string bookingCode = "";
                                    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                                    for(int k = 0; k < 5; ++k) bookingCode += charset[rand() % 36];

                                    // FIX: Chia đều số điểm đã dùng cho các vé trong giỏ hàng
                                    int pointsUsedPerTicket = 0;
                                    if (cartRows.size() > 0) {
                                        pointsUsedPerTicket = pointsToUse / cartRows.size();
                                    }

                                    for (size_t i = 0; i < cartRows.size(); i++) {
                                        double discountedTicketPrice = cartPrices[i] * (finalPrice / totalBatchPrice);
                                        // Ghi thêm biến pointsUsedPerTicket vào hàm saveBooking
                                        bookingMgr.saveBooking(loggedInUser->getUserID(), selectedFilmId, selectedTime, cartRows[i], cartCols[i], discountedTicketPrice, bookingCode, pointsUsedPerTicket);
                                    }

                                    // [ALGORITHM] UPDATE LOYALTY POINTS (Tiêu 1000đ = 1 điểm)
                                    int earnedPoints = (int)(finalPrice / 1000.0);
                                    int newTotalPoints = currentPoints - pointsToUse + earnedPoints;
                                    
                                    loggedInUser->setPoints(newTotalPoints);
                                    auth.updatePointsInFile(loggedInUser->getUserID(), newTotalPoints); // Lưu xuống file

                                    std::cout << "\n============================================\n";
                                    std::cout << "=> CONGRATULATIONS! Transaction successful.\n";
                                    std::cout << "=> YOUR BOOKING CODE IS: [" << bookingCode << "]\n";
                                    std::cout << "=> You earned +" << earnedPoints << " points for this purchase!\n";
                                    std::cout << "============================================\n";
                                    map.displayMap(); 
                                } else { 
                                    std::cout << "=> Transaction canceled. Returning to main menu...\n";
                                }
                            }
                            else if (subChoice == 3) { 
                                BookingManager bookingMgr;
                                bookingMgr.displayBookingHistory(loggedInUser->getUserID(), catalog);
                            }
                            else if (subChoice == 4) { 
                                ReviewManager reviewMgr;
                                reviewMgr.leaveReview(loggedInUser->getUserID(), catalog);
                            }
                            else if (subChoice == 5) { 
                                BookingManager bookingMgr;
                                int netPointsChange = bookingMgr.cancelBooking(loggedInUser->getUserID(), catalog);
                                
                                if (netPointsChange != 0) {
                                    int currentPts = loggedInUser->getPoints();
                                    int newPts = currentPts + netPointsChange; // Cộng gộp sự thay đổi
                                    if (newPts < 0) newPts = 0; // Chống âm điểm
                                    
                                    loggedInUser->setPoints(newPts);
                                    auth.updatePointsInFile(loggedInUser->getUserID(), newPts);
                                    
                                    if (netPointsChange > 0) {
                                        std::cout << "=> Refunded " << netPointsChange << " points. Your new point balance is: " << newPts << " Pts.\n";
                                    } else {
                                        std::cout << "=> Deducted " << -netPointsChange << " points. Your new point balance is: " << newPts << " Pts.\n";
                                    }
                                }
                            }
                            else if (subChoice == 6) { 
                                std::cout << "\n--- MEMBERSHIP PROFILE ---\n";
                                std::cout << "Account ID : " << loggedInUser->getUserID() << "\n";
                                std::cout << "Username   : " << loggedInUser->getUsername() << "\n";
                                std::cout << "Total Points: " << loggedInUser->getPoints() << " Pts\n";
                                std::cout << "Equivalent : " << (loggedInUser->getPoints() / 10) * 1000 << " VND discount available\n";
                                system("pause");
                            }
                        }
                    } 
                }
                break; 
            }
            case 2:
                // [FILE I/O] Registers user and appends to users.csv
                auth.registerUser();
                break;
            case 0:
                std::cout << "Saving data... Goodbye!\n";
                // [FILE I/O] Flushes all modifications back to films.csv before program termination
                catalog.saveFilmsToFile(); 
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}