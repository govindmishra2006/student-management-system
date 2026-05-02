// student.cpp  –  Implementation of StudentManager
// Full CRUD, sorting, file I/O, validation, and display logic

#include "student.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

// ─────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────
StudentManager::StudentManager(const std::string& filename)
    : dataFile_(filename)
{
    loadFromFile();   // Attempt to restore persisted data at startup
}

// ─────────────────────────────────────────────────────────
//  CRUD – Add
// ─────────────────────────────────────────────────────────
bool StudentManager::addStudent(const Student& s)
{
    if (idExists(s.id)) {
        std::cerr << "  [ERROR] Student with ID " << s.id << " already exists.\n";
        return false;
    }
    if (emailExists(s.email)) {
        std::cerr << "  [ERROR] Email '" << s.email << "' is already registered.\n";
        return false;
    }
    students_.push_back(s);
    if (!saveToFile()) {
        std::cerr << "  [WARNING] Record added in memory but could not persist to disk.\n";
    }
    return true;
}

// ─────────────────────────────────────────────────────────
//  CRUD – Update
// ─────────────────────────────────────────────────────────
bool StudentManager::updateStudent(int id, const Student& updated)
{
    for (auto& s : students_) {
        if (s.id == id) {
            // Email uniqueness check, excluding the current student
            if (s.email != updated.email && emailExists(updated.email, id)) {
                std::cerr << "  [ERROR] Email '" << updated.email << "' is taken by another student.\n";
                return false;
            }
            s.name   = updated.name;
            s.email  = updated.email;
            s.phone  = updated.phone;
            s.course = updated.course;
            s.gpa    = updated.gpa;
            s.age    = updated.age;
            saveToFile();
            return true;
        }
    }
    std::cerr << "  [ERROR] No student found with ID " << id << ".\n";
    return false;
}

// ─────────────────────────────────────────────────────────
//  CRUD – Delete
// ─────────────────────────────────────────────────────────
bool StudentManager::deleteStudent(int id)
{
    auto it = std::remove_if(students_.begin(), students_.end(),
                             [id](const Student& s) { return s.id == id; });
    if (it == students_.end()) {
        std::cerr << "  [ERROR] No student found with ID " << id << ".\n";
        return false;
    }
    students_.erase(it, students_.end());
    saveToFile();
    return true;
}

// ─────────────────────────────────────────────────────────
//  Search – by ID (returns pointer or nullptr)
// ─────────────────────────────────────────────────────────
Student* StudentManager::findById(int id)
{
    for (auto& s : students_) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

// ─────────────────────────────────────────────────────────
//  Search – by Name (case-insensitive substring match)
// ─────────────────────────────────────────────────────────
std::vector<Student*> StudentManager::findByName(const std::string& name)
{
    std::vector<Student*> results;
    std::string query = name;
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);

    for (auto& s : students_) {
        std::string lower = s.name;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower.find(query) != std::string::npos) {
            results.push_back(&s);
        }
    }
    return results;
}

// ─────────────────────────────────────────────────────────
//  Display – Table Header
// ─────────────────────────────────────────────────────────
void StudentManager::displayTableHeader() const
{
    const std::string divider(100, '-');
    std::cout << "\n" << divider << "\n";
    std::cout << std::left
              << std::setw(6)  << "ID"
              << std::setw(22) << "Name"
              << std::setw(28) << "Email"
              << std::setw(14) << "Phone"
              << std::setw(18) << "Course"
              << std::setw(6)  << "GPA"
              << std::setw(5)  << "Age"
              << "\n";
    std::cout << divider << "\n";
}

// ─────────────────────────────────────────────────────────
//  Display – Single Table Row
// ─────────────────────────────────────────────────────────
void StudentManager::displayTableRow(const Student& s) const
{
    std::cout << std::left
              << std::setw(6)  << s.id
              << std::setw(22) << s.name.substr(0, 20)
              << std::setw(28) << s.email.substr(0, 26)
              << std::setw(14) << s.phone
              << std::setw(18) << s.course.substr(0, 16)
              << std::setw(6)  << std::fixed << std::setprecision(2) << s.gpa
              << std::setw(5)  << s.age
              << "\n";
}

// ─────────────────────────────────────────────────────────
//  Display – Full detail card for a single student
// ─────────────────────────────────────────────────────────
void StudentManager::displayStudent(const Student& s) const
{
    std::cout << "\n  ┌─────────────────────────────────────┐\n";
    std::cout << "  │        STUDENT RECORD DETAILS       │\n";
    std::cout << "  ├─────────────────────────────────────┤\n";
    std::cout << "  │  ID      : " << std::left << std::setw(26) << s.id     << "│\n";
    std::cout << "  │  Name    : " << std::setw(26) << s.name                << "│\n";
    std::cout << "  │  Email   : " << std::setw(26) << s.email               << "│\n";
    std::cout << "  │  Phone   : " << std::setw(26) << s.phone               << "│\n";
    std::cout << "  │  Course  : " << std::setw(26) << s.course              << "│\n";
    std::cout << "  │  GPA     : " << std::setw(26) << std::fixed
              << std::setprecision(2) << s.gpa                               << "│\n";
    std::cout << "  │  Age     : " << std::setw(26) << s.age                 << "│\n";
    std::cout << "  └─────────────────────────────────────┘\n";
}

// ─────────────────────────────────────────────────────────
//  Display – All students in tabular format
// ─────────────────────────────────────────────────────────
void StudentManager::displayAll() const
{
    if (students_.empty()) {
        std::cout << "  [INFO] No student records found.\n";
        return;
    }
    displayTableHeader();
    for (const auto& s : students_) {
        displayTableRow(s);
    }
    std::cout << std::string(100, '-') << "\n";
    std::cout << "  Total records: " << students_.size() << "\n\n";
}

// ─────────────────────────────────────────────────────────
//  Sorting
// ─────────────────────────────────────────────────────────
void StudentManager::sortById(bool ascending)
{
    std::sort(students_.begin(), students_.end(),
              [ascending](const Student& a, const Student& b) {
                  return ascending ? a.id < b.id : a.id > b.id;
              });
    saveToFile();
}

void StudentManager::sortByName(bool ascending)
{
    std::sort(students_.begin(), students_.end(),
              [ascending](const Student& a, const Student& b) {
                  return ascending ? a.name < b.name : a.name > b.name;
              });
    saveToFile();
}

void StudentManager::sortByGpa(bool ascending)
{
    std::sort(students_.begin(), students_.end(),
              [ascending](const Student& a, const Student& b) {
                  return ascending ? a.gpa < b.gpa : a.gpa > b.gpa;
              });
    saveToFile();
}

// ─────────────────────────────────────────────────────────
//  Statistics
// ─────────────────────────────────────────────────────────
void StudentManager::showStatistics() const
{
    if (students_.empty()) {
        std::cout << "  [INFO] No data to compute statistics.\n";
        return;
    }

    float totalGpa = 0.0f, maxGpa = students_[0].gpa, minGpa = students_[0].gpa;
    int   totalAge = 0,    maxAge = students_[0].age, minAge = students_[0].age;
    std::string topStudent, lowStudent;

    for (const auto& s : students_) {
        totalGpa += s.gpa;
        totalAge += s.age;
        if (s.gpa > maxGpa) { maxGpa = s.gpa; topStudent = s.name; }
        if (s.gpa < minGpa) { minGpa = s.gpa; lowStudent = s.name; }
        if (s.age > maxAge) maxAge = s.age;
        if (s.age < minAge) minAge = s.age;
    }

    float avgGpa = totalGpa / students_.size();
    float avgAge = static_cast<float>(totalAge) / students_.size();

    // Count by GPA bracket
    int excellent = 0, good = 0, average = 0, below = 0;
    for (const auto& s : students_) {
        if      (s.gpa >= 3.5f) excellent++;
        else if (s.gpa >= 3.0f) good++;
        else if (s.gpa >= 2.0f) average++;
        else                    below++;
    }

    std::cout << "\n  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║         SYSTEM STATISTICS            ║\n";
    std::cout << "  ╠══════════════════════════════════════╣\n";
    std::cout << "  ║  Total Students  : " << std::left << std::setw(17) << students_.size() << "║\n";
    std::cout << "  ║  Average GPA     : " << std::setw(17) << std::fixed << std::setprecision(2) << avgGpa << "║\n";
    std::cout << "  ║  Highest GPA     : " << std::setw(17) << maxGpa << "║\n";
    std::cout << "  ║  Lowest GPA      : " << std::setw(17) << minGpa << "║\n";
    std::cout << "  ║  Average Age     : " << std::setw(17) << std::setprecision(1) << avgAge << "║\n";
    std::cout << "  ║  Age Range       : " << minAge << " - " << std::setw(12) << maxAge << "║\n";
    std::cout << "  ╠══════════════════════════════════════╣\n";
    std::cout << "  ║        GPA DISTRIBUTION              ║\n";
    std::cout << "  ╠══════════════════════════════════════╣\n";
    std::cout << "  ║  Excellent (≥3.5): " << std::setw(17) << excellent << "║\n";
    std::cout << "  ║  Good (3.0-3.49) : " << std::setw(17) << good     << "║\n";
    std::cout << "  ║  Average (2.0-2.99): " << std::setw(15) << average << "║\n";
    std::cout << "  ║  Below Avg (<2.0): " << std::setw(17) << below    << "║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n\n";
}

// ─────────────────────────────────────────────────────────
//  File I/O – Load  (CSV: id,name,email,phone,course,gpa,age)
// ─────────────────────────────────────────────────────────
bool StudentManager::loadFromFile()
{
    std::ifstream file(dataFile_);
    if (!file.is_open()) return false;   // Fresh start if no file exists

    students_.clear();
    std::string line;
    std::getline(file, line);            // Skip CSV header row

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string token;
        Student s;

        try {
            std::getline(ss, token, ','); s.id     = std::stoi(token);
            std::getline(ss, s.name,  ',');
            std::getline(ss, s.email, ',');
            std::getline(ss, s.phone, ',');
            std::getline(ss, s.course,',');
            std::getline(ss, token,   ','); s.gpa   = std::stof(token);
            std::getline(ss, token,   ','); s.age   = std::stoi(token);
            students_.push_back(s);
        } catch (const std::exception&) {
            // Skip malformed lines silently
        }
    }
    return true;
}

// ─────────────────────────────────────────────────────────
//  File I/O – Save
// ─────────────────────────────────────────────────────────
bool StudentManager::saveToFile() const
{
    std::ofstream file(dataFile_);
    if (!file.is_open()) return false;

    // CSV header
    file << "id,name,email,phone,course,gpa,age\n";

    for (const auto& s : students_) {
        file << s.id     << ","
             << s.name   << ","
             << s.email  << ","
             << s.phone  << ","
             << s.course << ","
             << std::fixed << std::setprecision(2) << s.gpa << ","
             << s.age    << "\n";
    }
    return true;
}

// ─────────────────────────────────────────────────────────
//  Validation – Email (basic RFC pattern)
// ─────────────────────────────────────────────────────────
bool StudentManager::isValidEmail(const std::string& email)
{
    const std::regex pattern(R"(^[a-zA-Z0-9._%+\-]+@[a-zA-Z0-9.\-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

// ─────────────────────────────────────────────────────────
//  Validation – Phone (10-15 digits, optional leading +)
// ─────────────────────────────────────────────────────────
bool StudentManager::isValidPhone(const std::string& phone)
{
    const std::regex pattern(R"(^\+?[0-9]{10,15}$)");
    return std::regex_match(phone, pattern);
}

// ─────────────────────────────────────────────────────────
//  Validation – GPA (0.00 – 4.00)
// ─────────────────────────────────────────────────────────
bool StudentManager::isValidGpa(float gpa)
{
    return gpa >= 0.0f && gpa <= 4.0f;
}

// ─────────────────────────────────────────────────────────
//  Validation – Age (10 – 100)
// ─────────────────────────────────────────────────────────
bool StudentManager::isValidAge(int age)
{
    return age >= 10 && age <= 100;
}

// ─────────────────────────────────────────────────────────
//  Validation – Name (non-empty, letters + spaces only)
// ─────────────────────────────────────────────────────────
bool StudentManager::isValidName(const std::string& name)
{
    if (name.empty() || name.length() > 50) return false;
    for (char c : name) {
        if (!std::isalpha(c) && c != ' ' && c != '-' && c != '\'') return false;
    }
    return true;
}

// ─────────────────────────────────────────────────────────
//  Helper – Auto-increment next ID
// ─────────────────────────────────────────────────────────
int StudentManager::getNextId(const std::vector<Student>& students)
{
    if (students.empty()) return 1001;
    int maxId = 0;
    for (const auto& s : students) {
        if (s.id > maxId) maxId = s.id;
    }
    return maxId + 1;
}

// ─────────────────────────────────────────────────────────
//  Private Helpers
// ─────────────────────────────────────────────────────────
bool StudentManager::idExists(int id) const
{
    for (const auto& s : students_) {
        if (s.id == id) return true;
    }
    return false;
}

bool StudentManager::emailExists(const std::string& email, int excludeId) const
{
    for (const auto& s : students_) {
        if (s.id != excludeId && s.email == email) return true;
    }
    return false;
}
