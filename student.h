#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

// ============================================================
//  Student Data Model
//  Represents a single student record with all attributes
// ============================================================
struct Student {
    int         id;
    std::string name;
    std::string email;
    std::string phone;
    std::string course;
    float       gpa;
    int         age;

    // Default constructor
    Student()
        : id(0), gpa(0.0f), age(0) {}

    // Parameterized constructor
    Student(int id, const std::string& name, const std::string& email,
            const std::string& phone, const std::string& course,
            float gpa, int age)
        : id(id), name(name), email(email),
          phone(phone), course(course), gpa(gpa), age(age) {}
};

// ============================================================
//  StudentManager Class Declaration
//  All CRUD, file I/O, sorting, and validation logic lives here
// ============================================================
class StudentManager {
public:
    StudentManager(const std::string& filename);

    // CRUD operations
    bool addStudent(const Student& s);
    bool updateStudent(int id, const Student& updated);
    bool deleteStudent(int id);

    // Search & display
    Student*              findById(int id);
    std::vector<Student*> findByName(const std::string& name);
    void                  displayAll() const;
    void                  displayStudent(const Student& s) const;
    void                  displayTableHeader() const;
    void                  displayTableRow(const Student& s) const;

    // Sorting
    void sortById(bool ascending = true);
    void sortByName(bool ascending = true);
    void sortByGpa(bool ascending = true);

    // Statistics
    void showStatistics() const;

    // File I/O
    bool loadFromFile();
    bool saveToFile() const;

    // Validation helpers (static, reusable)
    static bool   isValidEmail(const std::string& email);
    static bool   isValidPhone(const std::string& phone);
    static bool   isValidGpa(float gpa);
    static bool   isValidAge(int age);
    static bool   isValidName(const std::string& name);
    static int    getNextId(const std::vector<Student>& students);

    // Getters
    size_t count() const { return students_.size(); }

private:
    std::string          dataFile_;
    std::vector<Student> students_;

    // Internal helpers
    bool idExists(int id) const;
    bool emailExists(const std::string& email, int excludeId = -1) const;
};

#endif // STUDENT_H
