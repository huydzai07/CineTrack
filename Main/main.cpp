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

// Turn commas into space to avoid breaking the csv structrue
void sanitizeCSV(std::string& input) {
    for (char& c : input) {
        if (c == ',') c = ' ';
    }
}


void displayMainMenu() {
    std::cout << "\n=== CINETRACK SYSTEM ===\n";
    std::cout << "1. Login\n";
    std::cout << "2. Register\n";
    std::cout << "0. Exit\n";
    std::cout << "Select option: ";
}

int main() {
    // Initializes the random number generator for booking codes
    srand(time(0));
    
    // [FILE I/O] Initialize system managers and preload data from CSV files into RAM
    AuthManager auth;
    auth.loadUsersFromFile();
    
    FilmCatalog catalog; 
    catalog.loadFilmsFromFile(); 

    int choice = -1;
    // Main execution loop: Displays menu and handles primary navigation
    while (choice != 0) {
        // [INPUT VALIDATION] Locks the Main Menu flow, strictly requiring 0, 1, or 2
        while (true) {
            system("cls"); // Clears the console for a minimalist UI experience
            displayMainMenu();
            std::cin >> choice;

            // Prevents infinite loops and system crashes if the user inputs a char/string instead of an int
            if (std::cin.fail() || choice < 0 || choice > 2) {
                std::cin.clear(); // Clear the error flag
                std::cin.ignore(10000, '\n'); // Discard invalid characters in the input buffer
                std::cout << "=> Error: Invalid option! Please enter exactly 0, 1, or 2.\n";
                system("pause"); 
            } else {
                std::cin.ignore(10000, '\n'); // Clears any remaining whitespace in the buffer
                break; 
            }
        }

        switch (choice) {
            case 1: { 
                system("cls");
                // [OOP & MEMORY] Polymorphism applied here using std::shared_ptr for safe memory management
                std::shared_ptr<User> loggedInUser = auth.login(); 
                
                if (loggedInUser != nullptr) {
                    int subChoice = -1;
                    while (subChoice != 0) {
                        
                        // [INPUT VALIDATION] Locks the Sub-Menu flow for Admin/Customer to valid ranges (0-6)
                        while (true) {
                            loggedInUser->displayMenu(); // Dynamic binding: calls Admin or Customer menu
                            std::cout << "Select option: ";
                            std::cin >> subChoice;

                            if (std::cin.fail() || subChoice < 0 || subChoice > 6) {
                                std::cin.clear(); 
                                std::cin.ignore(10000, '\n'); 
                                std::cout << "=> Error: Invalid option! Please enter a number between 0 and 6.\n";
                                system("pause");
                                system("cls"); 
                            } else {
                                std::cin.ignore(10000, '\n'); 
                                break;
                            }
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

                                if (catalog.getFilmTitleById(id) != "Unknown Film") {
                                    std::cout << "=> Error: This Film ID already exists in the system!\n";
                                    system("pause");
                                    continue;
                                }
                                
                                std::cin.ignore(); 
                                std::cout << "Enter Title: ";
                                std::getline(std::cin, title);
                                sanitizeCSV(title);
                                std::cout << "Enter Genre: ";
                                std::getline(std::cin, genre);
                                sanitizeCSV(genre);
                                
                                // [INPUT VALIDATION] Re-prompts until a valid numerical year is entered
                                while (true) {
                                    std::cout << "Enter Year: ";
                                    std::cin >> year;
                                    if (std::cin.fail() || year < 1800 || year > 2100) {
                                        std::cin.clear(); std::cin.ignore(10000, '\n');
                                        std::cout << "=> Error: Invalid year! Please enter a valid number (1800-2100).\n";
                                    } else {
                                        std::cin.ignore(10000, '\n');
                                        break;
                                    }
                                }
                                
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
                                // Loop allows adding multiple showtimes continuously without returning to main menu
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
                                
                                // [INPUT VALIDATION] Strict boundary check for movie ratings
                                while (true) {
                                    std::cout << "Enter new rating (0.0 - 5.0): ";
                                    std::cin >> newRate;
                                    if (std::cin.fail() || newRate < 0.0 || newRate > 5.0) {
                                        std::cin.clear(); std::cin.ignore(10000, '\n');
                                        std::cout << "=> Error: Invalid rating! Please enter a number between 0.0 and 5.0.\n";
                                    } else {
                                        std::cin.ignore(10000, '\n');
                                        break;
                                    }
                                }
                                
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
                                sanitizeCSV(newTitle);
                                std::cout << "Enter new genre: ";
                                std::getline(std::cin, newGenre);
                                sanitizeCSV(newGenre);
                                
                                // [INPUT VALIDATION] Re-prompts until a valid numerical year is entered
                                while (true) {
                                    std::cout << "Enter new year: ";
                                    std::cin >> newYear;
                                    if (std::cin.fail() || newYear < 1800 || newYear > 2100) {
                                        std::cin.clear(); std::cin.ignore(10000, '\n');
                                        std::cout << "=> Error: Invalid year! Please enter a valid number (1800-2100).\n";
                                    } else {
                                        std::cin.ignore(10000, '\n');
                                        break;
                                    }
                                }
                                
                                catalog.editFilm(searchId, newTitle, newGenre, newYear);
                            }
                            else if (subChoice == 5) { 
                                std::string searchId;
                                std::cout << "\n-- DELETE FILM --\n";
                                catalog.displayWithFilter();
                                
                                std::cout << "\nEnter Film ID to permanently delete or '0' to cancel: ";
                                std::cin >> searchId;
                                if (searchId == "0") { std::cout << "=> Operation canceled.\n"; continue; }
                                
                                // [MEMORY] Deletes object safely from the vector of unique_ptrs
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
                                // [ALGORITHM] Linear search & sorting algorithm execution
                                catalog.advancedSearch(); 
                            }
                            else if (subChoice == 2) { 
                                std::cout << "\n-- BOOK TICKETS --\n";
                                catalog.displayWithFilter();
                                
                                std::string selectedFilmId;
                                std::string filmTitle;
                                
                                // Locks flow until a valid existing Film ID is inputted
                                while (true) {
                                    std::cout << "\nEnter Film ID you want to watch or '0' to cancel: ";
                                    std::cin >> selectedFilmId;
                                    if (selectedFilmId == "0") break;

                                    filmTitle = catalog.getFilmTitleById(selectedFilmId);
                                    if (filmTitle == "Unknown Film") {
                                        std::cout << "=> Error: Film ID does not exist in the system! Please try again.\n";
                                    } else {
                                        break;
                                    }
                                }

                                if (selectedFilmId == "0") { 
                                    std::cout << "=> Operation canceled.\n"; 
                                    system("pause");
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

                                // [FILE I/O & ALGORITHM] Load 2D Seat Map representation from persistent storage
                                SeatMap map;
                                BookingManager bookingMgr;
                                std::cout << "\n=> Loading seat map for film [" << filmTitle << "] - Showtime [" << selectedTime << "]...\n";
                                
                                bookingMgr.loadBookings(selectedFilmId, selectedTime, map);
                                map.displayMap();

                                int availableSeats = map.getAvailableSeats();

                                // Nếu rạp đã kín chỗ, chặn ngay từ đầu không cho mua nữa
                                if (availableSeats == 0) {
                                    std::cout << "=> We're sorry, this showtime is fully booked (Sold out)!\n";
                                    system("pause");
                                    continue; // Quay trở lại menu
                                }

                                int numTickets = 0;
                                // [INPUT VALIDATION] Ensures positive integer and checks against capacity
                                while (true) {
                                    // Cập nhật câu prompt để khách hàng biết rạp còn bao nhiêu chỗ
                                    std::cout << "\nEnter number of tickets to buy (or '0' to cancel) - [" << availableSeats << " seats left]: ";
                                    std::cin >> numTickets;
                                    
                                    if (std::cin.fail() || numTickets < 0) {
                                        std::cin.clear(); 
                                        std::cin.ignore(10000, '\n'); 
                                        std::cout << "=> Error: Number of tickets must be a valid positive integer!\n";
                                    } 
                                    // Chặn luồng nếu khách nhập số vé > số ghế trống
                                    else if (numTickets > availableSeats) {
                                        std::cout << "=> Error: Not enough seats available! The theater only has " << availableSeats << " seats left.\n";
                                    } 
                                    else {
                                        break; // Số lượng hợp lệ, thoát vòng lặp để tiếp tục thanh toán
                                    }
                                }

                                if (numTickets == 0) {
                                    std::cout << "=> Ticket booking canceled.\n";
                                    continue; 
                                }

                                // [ALGORITHM] Dynamic Pricing Module: Base price depends on user demographics (Age)
                                int userAge = loggedInUser->getAge();
                                double basePrice = 0.0;
                                if (userAge < 22) basePrice = 70000.0;
                                else if (userAge >= 22 && userAge <= 70) basePrice = 120000.0;
                                else basePrice = 50000.0;

                                // [DATA STRUCTURES] STL Vectors act as a temporary "Shopping Cart" for batch booking
                                std::vector<char> cartRows;
                                std::vector<int> cartCols;
                                std::vector<double> cartPrices;
                                std::vector<std::string> cartNames;
                                double totalBatchPrice = 0.0;

                                // Iterates to collect the requested number of valid seats
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

                                        if (std::cin.fail()) {
                                            std::cin.clear(); // 1. Mở khóa lại cin sau khi sập
                                            std::cin.ignore(10000, '\n'); // 2. Vứt bỏ toàn bộ chữ rác (như chữ 'f') đang kẹt trong buffer
                                            std::cout << "=> Error: Invalid input format! Please enter a letter for row and a number for seat.\n";
                                            continue; // 3. Bỏ qua các lệnh bên dưới, quay lại đầu vòng lặp cho nhập lại
                                        }

                                        // Validates seat boundaries and temporarily marks it as [X] in RAM
                                        if (map.bookSeat(row, col)) {
                                            seatValid = true;
                                            double surcharge = 0.0;
                                            std::string seatType = "";
                                            char upperRow = toupper(row);

                                            // [ALGORITHM] Spatial Zoning: Surcharge mapping based on seat tier
                                            if (upperRow >= 'A' && upperRow <= 'D') {
                                                seatType = "Regular"; surcharge = 0.0;
                                            } else if (upperRow >= 'E' && upperRow <= 'H') {
                                                seatType = "VIP"; surcharge = 80000.0;
                                            } else {
                                                seatType = "Deluxe"; surcharge = 130000.0;
                                            }

                                            double ticketPrice = basePrice + surcharge;
                                            totalBatchPrice += ticketPrice;

                                            // Pushes valid selections into the vector cart
                                            cartRows.push_back(upperRow);
                                            cartCols.push_back(col);
                                            cartPrices.push_back(ticketPrice);
                                            cartNames.push_back(std::string(1, upperRow) + std::to_string(col) + " [" + seatType + "]");
                                        } else {
                                            std::cout << "=> Error: Seat does not exist or is already booked! Please choose again.\n";
                                        }
                                    }
                                    if (row == '0') {
                                        cartRows.clear(); // Clears cart to prevent partial transactions if canceled midway
                                        break; 
                                    }
                                }

                                if (cartRows.empty()) continue; 

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

                                // [LOYALTY SYSTEM] Point redemption threshold: 10 points = 1000 VND
                                if (currentPoints >= 10) {
                                    int maxDiscountVND = (currentPoints / 10) * 1000;
                                    std::cout << "=> You currently have " << currentPoints << " Membership Points.\n";
                                    std::cout << "=> (Rate: 10 Points = 1000 VND Discount. Max usable: " << maxDiscountVND << " VND).\n";
                                    
                                    // [INPUT VALIDATION] Ensures point redemption does not exceed available balance
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

                                    // Floors points to multiples of 10 for structured calculation
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
                                    // [ALGORITHM] Generates a 5-character alphanumeric booking reference code
                                    std::string bookingCode = "";
                                    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                                    for(int k = 0; k < 5; ++k) bookingCode += charset[rand() % 36];

                                    // Equitably distributes redeemed points across all tickets in the batch
                                    int pointsUsedPerTicket = 0;
                                    if (cartRows.size() > 0) {
                                        pointsUsedPerTicket = pointsToUse / cartRows.size();
                                    }

                                    // Flushes transaction array to persistent CSV storage
                                    for (size_t i = 0; i < cartRows.size(); i++) {
                                        double discountedTicketPrice = cartPrices[i] * (finalPrice / totalBatchPrice);
                                        bookingMgr.saveBooking(loggedInUser->getUserID(), selectedFilmId, selectedTime, cartRows[i], cartCols[i], discountedTicketPrice, bookingCode, pointsUsedPerTicket);
                                    }

                                    // [LOYALTY SYSTEM] Earns 1 point per 1000 VND spent in final transaction
                                    int earnedPoints = (int)(finalPrice / 1000.0);
                                    int newTotalPoints = currentPoints - pointsToUse + earnedPoints;
                                    
                                    loggedInUser->setPoints(newTotalPoints);
                                    auth.updatePointsInFile(loggedInUser->getUserID(), newTotalPoints); 

                                    std::cout << "\n============================================\n";
                                    std::cout << "=> CONGRATULATIONS! Transaction successful.\n";
                                    std::cout << "=> YOUR BOOKING CODE IS: [" << bookingCode << "]\n";
                                    std::cout << "=> You earned +" << earnedPoints << " points for this purchase!\n";
                                    std::cout << "============================================\n";
                                    map.displayMap(); 
                                    system("pause");
                                } else { 
                                    std::cout << "=> Transaction canceled. Returning to main menu...\n";
                                    system("pause");
                                }
                            }
                            else if (subChoice == 3) { 
                                BookingManager bookingMgr;
                                bookingMgr.displayBookingHistory(loggedInUser->getUserID(), catalog);
                                system("pause");
                            }
                            else if (subChoice == 4) { 
                                ReviewManager reviewMgr;
                                reviewMgr.leaveReview(loggedInUser->getUserID(), catalog);
                            }
                            else if (subChoice == 5) { 
                                BookingManager bookingMgr;
                                int netPointsChange = bookingMgr.cancelBooking(loggedInUser->getUserID(), catalog);
                                
                                // [LOYALTY SYSTEM] Recalculates point balance based on refund vs penalty algorithm
                                if (netPointsChange != 0) {
                                    int currentPts = loggedInUser->getPoints();
                                    int newPts = currentPts + netPointsChange; 
                                    if (newPts < 0) newPts = 0; // Prevents the point balance from dropping below zero
                                    
                                    loggedInUser->setPoints(newPts);
                                    auth.updatePointsInFile(loggedInUser->getUserID(), newPts);
                                    
                                    if (netPointsChange > 0) {
                                        std::cout << "=> Refunded " << netPointsChange << " points. Your new point balance is: " << newPts << " Pts.\n";
                                    } else {
                                        std::cout << "=> Deducted " << -netPointsChange << " points. Your new point balance is: " << newPts << " Pts.\n";
                                    }
                                }
                                system("pause");
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
                system("cls");
                // [FILE I/O] Registers user and appends credentials securely to users.csv
                auth.registerUser();
                break;
            case 0:
                std::cout << "Saving data... Goodbye!\n";
                // [FILE I/O] Flushes all state modifications back to films.csv before process termination
                catalog.saveFilmsToFile(); 
                break;
        }
    }
    return 0;
}
