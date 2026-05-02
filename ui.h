#ifndef UI_H
#define UI_H

#include "student.h"
#include <string>

// ============================================================
//  UI – Menu-driven console interface
//  All user input collection and menu rendering lives here
// ============================================================
class UI {
public:
    explicit UI(StudentManager& manager);

    // Entry point – runs the main loop
    void run();

private:
    StudentManager& mgr_;

    // Menu renderers
    void showMainMenu() const;
    void showSortMenu() const;

    // Feature handlers
    void handleAdd();
    void handleDisplay();
    void handleSearchById();
    void handleSearchByName();
    void handleUpdate();
    void handleDelete();
    void handleSort();
    void handleStatistics();

    // Input helpers
    int         getIntInput(const std::string& prompt, int min, int max);
    float       getFloatInput(const std::string& prompt, float min, float max);
    std::string getStringInput(const std::string& prompt, bool allowEmpty = false);
    bool        confirm(const std::string& message);

    // Validated field collectors
    std::string collectValidName(const std::string& defaultValue = "");
    std::string collectValidEmail(const std::string& defaultValue = "");
    std::string collectValidPhone(const std::string& defaultValue = "");
    float       collectValidGpa(float defaultValue = 0.0f);
    int         collectValidAge(int defaultValue = 0);

    // Display helpers
    void printBanner() const;
    void pressEnterToContinue() const;
    void clearScreen() const;
};

#endif // UI_H
