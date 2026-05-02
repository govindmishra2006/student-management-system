// ui.cpp  –  Full implementation of the console UI layer

#include "ui.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

// ─────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────
UI::UI(StudentManager& manager) : mgr_(manager) {}

// ─────────────────────────────────────────────────────────
//  Main loop
// ─────────────────────────────────────────────────────────
void UI::run()
{
    clearScreen();
    printBanner();

    while (true) {
        showMainMenu();
        int choice = getIntInput("  Enter your choice", 0, 8);

        switch (choice) {
            case 1: handleAdd();          break;
            case 2: handleDisplay();      break;
            case 3: handleSearchById();   break;
            case 4: handleSearchByName(); break;
            case 5: handleUpdate();       break;
            case 6: handleDelete();       break;
            case 7: handleSort();         break;
            case 8: handleStatistics();   break;
            case 0:
                std::cout << "\n  Goodbye! All records saved.\n\n";
                return;
        }
    }
}

// ─────────────────────────────────────────────────────────
//  Menus
// ─────────────────────────────────────────────────────────
void UI::showMainMenu() const
{
    std::cout << "\n  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║   STUDENT MANAGEMENT SYSTEM  v2.0   ║\n";
    std::cout << "  ╠══════════════════════════════════════╣\n";
    std::cout << "  ║  1. Add Student                      ║\n";
    std::cout << "  ║  2. Display All Students             ║\n";
    std::cout << "  ║  3. Search by ID                     ║\n";
    std::cout << "  ║  4. Search by Name                   ║\n";
    std::cout << "  ║  5. Update Student                   ║\n";
    std::cout << "  ║  6. Delete Student                   ║\n";
    std::cout << "  ║  7. Sort Records                     ║\n";
    std::cout << "  ║  8. Statistics                       ║\n";
    std::cout << "  ║  0. Exit                             ║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n";
}

void UI::showSortMenu() const
{
    std::cout << "\n  Sort by:\n";
    std::cout << "  1. ID (Ascending)\n";
    std::cout << "  2. ID (Descending)\n";
    std::cout << "  3. Name (A-Z)\n";
    std::cout << "  4. Name (Z-A)\n";
    std::cout << "  5. GPA (Highest first)\n";
    std::cout << "  6. GPA (Lowest first)\n";
}

// ─────────────────────────────────────────────────────────
//  Handler – Add Student
// ─────────────────────────────────────────────────────────
void UI::handleAdd()
{
    std::cout << "\n  ── ADD NEW STUDENT ─────────────────────\n";

    Student s;
    s.id     = StudentManager::getNextId({});   // Placeholder; real call below
    // We need the actual next ID from the manager
    // Workaround: collect all existing students count + generate
    // (manager exposes count(); we'll just use a simple approach)
    // The actual ID is auto-generated – we pass 0 and let addStudent handle it
    // Actually we generate via static helper after displaying suggestion:

    std::cout << "  (Suggested ID will be auto-assigned)\n";

    s.name   = collectValidName();
    s.email  = collectValidEmail();
    s.phone  = collectValidPhone();
    s.course = getStringInput("  Course          : ");
    if (s.course.empty()) { std::cout << "  [ERROR] Course cannot be empty.\n"; return; }
    s.gpa    = collectValidGpa();
    s.age    = collectValidAge();

    // Auto-assign ID (manager will reject duplicates anyway)
    s.id = 1000 + static_cast<int>(mgr_.count()) + 1;
    // Keep incrementing until unique
    while (mgr_.findById(s.id) != nullptr) s.id++;

    if (mgr_.addStudent(s)) {
        std::cout << "\n  [SUCCESS] Student added with ID: " << s.id << "\n";
        mgr_.displayStudent(s);
    }
    pressEnterToContinue();
}

// ─────────────────────────────────────────────────────────
//  Handler – Display All
// ─────────────────────────────────────────────────────────
void UI::handleDisplay()
{
    std::cout << "\n  ── ALL STUDENT RECORDS ─────────────────\n";
    mgr_.displayAll();
    pressEnterToContinue();
}

// ─────────────────────────────────────────────────────────
//  Handler – Search by ID
// ─────────────────────────────────────────────────────────
void UI::handleSearchById()
{
    std::cout << "\n  ── SEARCH BY ID ────────────────────────\n";
    int id = getIntInput("  Enter Student ID", 1, 999999);
    Student* s = mgr_.findById(id);
    if (s) {
        mgr_.displayStudent(*s);
    } else {
        std::cout << "  [NOT FOUND] No student with ID " << id << ".\n";
    }
    pressEnterToContinue();
}

// ─────────────────────────────────────────────────────────
//  Handler – Search by Name
// ─────────────────────────────────────────────────────────
void UI::handleSearchByName()
{
    std::cout << "\n  ── SEARCH BY NAME ──────────────────────\n";
    std::string name = getStringInput("  Enter name (or partial name): ");
    auto results = mgr_.findByName(name);

    if (results.empty()) {
        std::cout << "  [NOT FOUND] No students matching '" << name << "'.\n";
    } else {
        std::cout << "  Found " << results.size() << " match(es):\n";
        mgr_.displayTableHeader();
        for (auto* s : results) {
            mgr_.displayTableRow(*s);
        }
        std::cout << std::string(100, '-') << "\n";
    }
    pressEnterToContinue();
}

// ─────────────────────────────────────────────────────────
//  Handler – Update Student
// ─────────────────────────────────────────────────────────
void UI::handleUpdate()
{
    std::cout << "\n  ── UPDATE STUDENT ──────────────────────\n";
    int id = getIntInput("  Enter Student ID to update", 1, 999999);
    Student* existing = mgr_.findById(id);

    if (!existing) {
        std::cout << "  [NOT FOUND] No student with ID " << id << ".\n";
        pressEnterToContinue();
        return;
    }

    std::cout << "  Current record:\n";
    mgr_.displayStudent(*existing);
    std::cout << "  (Press ENTER to keep current value)\n\n";

    Student updated = *existing;   // Start with current values

    // Collect each field; empty input means keep existing
    std::cout << "  Name [" << existing->name << "]: ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) {
        while (!StudentManager::isValidName(input)) {
            std::cout << "  [ERROR] Invalid name. Re-enter: ";
            std::getline(std::cin, input);
            if (input.empty()) { input = existing->name; break; }
        }
        if (!input.empty()) updated.name = input;
    }

    std::cout << "  Email [" << existing->email << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        while (!StudentManager::isValidEmail(input)) {
            std::cout << "  [ERROR] Invalid email. Re-enter: ";
            std::getline(std::cin, input);
            if (input.empty()) { input = existing->email; break; }
        }
        if (!input.empty()) updated.email = input;
    }

    std::cout << "  Phone [" << existing->phone << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        while (!StudentManager::isValidPhone(input)) {
            std::cout << "  [ERROR] Invalid phone. Re-enter: ";
            std::getline(std::cin, input);
            if (input.empty()) { input = existing->phone; break; }
        }
        if (!input.empty()) updated.phone = input;
    }

    std::cout << "  Course [" << existing->course << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) updated.course = input;

    std::cout << "  GPA [" << existing->gpa << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            float g = std::stof(input);
            if (StudentManager::isValidGpa(g)) updated.gpa = g;
            else std::cout << "  [WARNING] Invalid GPA; keeping original.\n";
        } catch (...) {
            std::cout << "  [WARNING] Invalid input; keeping original GPA.\n";
        }
    }

    std::cout << "  Age [" << existing->age << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try {
            int a = std::stoi(input);
            if (StudentManager::isValidAge(a)) updated.age = a;
            else std::cout << "  [WARNING] Invalid age; keeping original.\n";
        } catch (...) {
            std::cout << "  [WARNING] Invalid input; keeping original age.\n";
        }
    }

    if (confirm("  Apply these changes?")) {
        if (mgr_.updateStudent(id, updated)) {
            std::cout << "  [SUCCESS] Student updated.\n";
            mgr_.displayStudent(updated);
        }
    } else {
        std::cout << "  Update cancelled.\n";
    }
    pressEnterToContinue();
}

// ─────────────────────────────────────────────────────────
//  Handler – Delete Student
// ─────────────────────────────────────────────────────────
void UI::handleDelete()
{
    std::cout << "\n  ── DELETE STUDENT ──────────────────────\n";
    int id = getIntInput("  Enter Student ID to delete", 1, 999999);
    Student* s = mgr_.findById(id);

    if (!s) {
        std::cout << "  [NOT FOUND] No student with ID " << id << ".\n";
        pressEnterToContinue();
        return;
    }

    mgr_.displayStudent(*s);
    if (confirm("  Are you sure you want to DELETE this record? This is irreversible.")) {
        if (mgr_.deleteStudent(id)) {
            std::cout << "  [SUCCESS] Student ID " << id << " deleted.\n";
        }
    } else {
        std::cout << "  Deletion cancelled.\n";
    }
    pressEnterToContinue();
}

// ─────────────────────────────────────────────────────────
//  Handler – Sort
// ─────────────────────────────────────────────────────────
void UI::handleSort()
{
    showSortMenu();
    int choice = getIntInput("  Choose sort option", 1, 6);

    switch (choice) {
        case 1: mgr_.sortById(true);    std::cout << "  Sorted by ID (asc).\n";       break;
        case 2: mgr_.sortById(false);   std::cout << "  Sorted by ID (desc).\n";      break;
        case 3: mgr_.sortByName(true);  std::cout << "  Sorted by Name (A-Z).\n";     break;
        case 4: mgr_.sortByName(false); std::cout << "  Sorted by Name (Z-A).\n";     break;
        case 5: mgr_.sortByGpa(false);  std::cout << "  Sorted by GPA (highest).\n";  break;
        case 6: mgr_.sortByGpa(true);   std::cout << "  Sorted by GPA (lowest).\n";   break;
    }
    mgr_.displayAll();
    pressEnterToContinue();
}

// ─────────────────────────────────────────────────────────
//  Handler – Statistics
// ─────────────────────────────────────────────────────────
void UI::handleStatistics()
{
    std::cout << "\n  ── STATISTICS ──────────────────────────\n";
    mgr_.showStatistics();
    pressEnterToContinue();
}

// ─────────────────────────────────────────────────────────
//  Input helpers
// ─────────────────────────────────────────────────────────
int UI::getIntInput(const std::string& prompt, int min, int max)
{
    int value;
    while (true) {
        std::cout << prompt << " (" << min << "-" << max << "): ";
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [ERROR] Please enter a number between " << min << " and " << max << ".\n";
    }
}

float UI::getFloatInput(const std::string& prompt, float min, float max)
{
    float value;
    while (true) {
        std::cout << prompt << " (" << min << "-" << max << "): ";
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [ERROR] Please enter a number between " << min << " and " << max << ".\n";
    }
}

std::string UI::getStringInput(const std::string& prompt, bool allowEmpty)
{
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        // Trim leading/trailing whitespace
        size_t start = value.find_first_not_of(" \t");
        size_t end   = value.find_last_not_of(" \t");
        value = (start == std::string::npos) ? "" : value.substr(start, end - start + 1);

        if (allowEmpty || !value.empty()) return value;
        std::cout << "  [ERROR] Input cannot be empty.\n";
    }
}

bool UI::confirm(const std::string& message)
{
    std::cout << message << " (y/n): ";
    char c;
    std::cin >> c;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return (c == 'y' || c == 'Y');
}

// ─────────────────────────────────────────────────────────
//  Validated field collectors
// ─────────────────────────────────────────────────────────
std::string UI::collectValidName(const std::string& /*defaultValue*/)
{
    while (true) {
        std::string name = getStringInput("  Full Name       : ");
        if (StudentManager::isValidName(name)) return name;
        std::cout << "  [ERROR] Name must be 1-50 chars, letters/spaces/hyphens only.\n";
    }
}

std::string UI::collectValidEmail(const std::string& /*defaultValue*/)
{
    while (true) {
        std::string email = getStringInput("  Email           : ");
        if (StudentManager::isValidEmail(email)) return email;
        std::cout << "  [ERROR] Invalid email format (e.g. user@domain.com).\n";
    }
}

std::string UI::collectValidPhone(const std::string& /*defaultValue*/)
{
    while (true) {
        std::string phone = getStringInput("  Phone           : ");
        if (StudentManager::isValidPhone(phone)) return phone;
        std::cout << "  [ERROR] Phone must be 10-15 digits (optional leading +).\n";
    }
}

float UI::collectValidGpa(float /*defaultValue*/)
{
    return getFloatInput("  GPA (0.00-4.00) ", 0.0f, 4.0f);
}

int UI::collectValidAge(int /*defaultValue*/)
{
    return getIntInput("  Age             ", 10, 100);
}

// ─────────────────────────────────────────────────────────
//  Display helpers
// ─────────────────────────────────────────────────────────
void UI::printBanner() const
{
    std::cout << R"(
  ┌──────────────────────────────────────────────────────┐
  │                                                      │
  │       ADVANCED STUDENT MANAGEMENT SYSTEM v2.0        │
  │             Built with C++ Standard Library          │
  │                                                      │
  └──────────────────────────────────────────────────────┘
)";
}

void UI::pressEnterToContinue() const
{
    std::cout << "\n  Press ENTER to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (std::cin.peek() == '\n') std::cin.ignore();
    // Flush any leftover
    std::cout << "\n";
}

void UI::clearScreen() const
{
#ifdef _WIN32
    (void)std::system("cls");
#else
    (void)std::system("clear");
#endif
}
