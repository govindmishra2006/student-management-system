// main.cpp  –  Entry point for the Student Management System
//
// Initializes the StudentManager with a persistent CSV file,
// wires it into the UI, and starts the interactive loop.

#include "student.h"
#include "ui.h"

#include <iostream>
#include <stdexcept>

int main()
{
    try {
        // The data file is created automatically on first run
        StudentManager manager("students.csv");

        UI ui(manager);
        ui.run();

    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Unexpected error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
