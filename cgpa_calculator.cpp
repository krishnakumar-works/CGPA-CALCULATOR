/*
 * Task 1: CGPA Calculator (C++ version)
 * -------------------------------------
 * - Takes the number of courses taken by the student
 * - For each course: name, grade (letter or points) and credit hours
 * - Calculates total credits and total grade points (grade x credit hours)
 * - Computes the semester GPA and then the overall CGPA
 * - Displays individual course grades and the final CGPA
 *
 * Compile: g++ -std=c++17 cgpa_calculator.cpp -o cgpa_calculator
 * Run    : ./cgpa_calculator
 */

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Course {
    std::string name;
    std::string grade;      // what the user typed, e.g. "A-" or "3.5"
    double points = 0.0;    // numeric grade point (0.0 - 4.0)
    double credits = 0.0;   // credit hours
    double gradePoints() const { return points * credits; }  // grade x credit hours
};

struct Summary {
    double totalCredits = 0.0;
    double totalPoints = 0.0;
    double gpa = 0.0;
};

// Standard 4.0 grading scale
static const std::map<std::string, double> GRADE_SCALE = {
    {"A+", 4.0}, {"A", 4.0}, {"A-", 3.7},
    {"B+", 3.3}, {"B", 3.0}, {"B-", 2.7},
    {"C+", 2.3}, {"C", 2.0}, {"C-", 1.7},
    {"D+", 1.3}, {"D", 1.0}, {"F", 0.0}
};

// ---------- input helpers ----------

static std::string readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        std::cout << "\nInput ended unexpectedly.\n";
        std::exit(1);
    }
    return line;
}

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Parses the whole string as a double; returns false if it is not a clean number.
static bool parseDouble(const std::string& text, double& out) {
    try {
        size_t pos = 0;
        out = std::stod(text, &pos);
        return pos == text.size();
    } catch (...) {
        return false;
    }
}

static int readInt(const std::string& prompt, int minimum) {
    while (true) {
        std::string text = trim(readLine(prompt));
        try {
            size_t pos = 0;
            int value = std::stoi(text, &pos);
            if (pos != text.size())
                std::cout << "  Invalid input. Please enter a whole number.\n";
            else if (value < minimum)
                std::cout << "  Please enter a number >= " << minimum << ".\n";
            else
                return value;
        } catch (...) {
            std::cout << "  Invalid input. Please enter a whole number.\n";
        }
    }
}

static double readDouble(const std::string& prompt, double minimum, double maximum) {
    while (true) {
        double value;
        if (!parseDouble(trim(readLine(prompt)), value))
            std::cout << "  Invalid input. Please enter a number.\n";
        else if (value < minimum || value > maximum)
            std::cout << "  Value must be between " << minimum << " and " << maximum << ".\n";
        else
            return value;
    }
}

// Accepts a letter grade (A, B+, ...) or a numeric grade point (0-4).
static void readGrade(const std::string& prompt, std::string& gradeOut, double& pointsOut) {
    while (true) {
        std::string text = trim(readLine(prompt));
        std::transform(text.begin(), text.end(), text.begin(),
                       [](unsigned char c) { return std::toupper(c); });

        auto it = GRADE_SCALE.find(text);
        if (it != GRADE_SCALE.end()) {
            gradeOut = text;
            pointsOut = it->second;
            return;
        }

        double value;
        if (!text.empty() && parseDouble(text, value) && value >= 0.0 && value <= 4.0) {
            gradeOut = text;
            pointsOut = value;
            return;
        }
        std::cout << "  Invalid grade. Use a letter (A, B+, C-, ...) or a number 0-4.\n";
    }
}

// ---------- calculations ----------

static Summary calculateGPA(const std::vector<Course>& courses) {
    Summary s;
    for (const Course& c : courses) {
        s.totalCredits += c.credits;
        s.totalPoints += c.gradePoints();
    }
    s.gpa = (s.totalCredits > 0.0) ? s.totalPoints / s.totalCredits : 0.0;
    return s;
}

static double calculateCGPA(const Summary& sem, double prevCredits, double prevCgpa) {
    double allCredits = prevCredits + sem.totalCredits;
    double allPoints = prevCgpa * prevCredits + sem.totalPoints;
    return (allCredits > 0.0) ? allPoints / allCredits : 0.0;
}

// ---------- output ----------

static void displayReport(const std::vector<Course>& courses, const Summary& sem, double cgpa) {
    const std::string line(62, '=');
    const std::string dash(62, '-');

    std::cout << "\n" << line << "\n"
              << std::left << std::setw(22) << "Course" << std::setw(8) << "Grade"
              << std::setw(9) << "Points" << std::setw(9) << "Credits"
              << std::setw(10) << "Grade Pts" << "\n"
              << dash << "\n" << std::fixed;

    for (const Course& c : courses) {
        std::cout << std::left << std::setw(22) << c.name.substr(0, 21)
                  << std::setw(8) << c.grade
                  << std::setw(9) << std::setprecision(2) << c.points
                  << std::setw(9) << std::setprecision(1) << c.credits
                  << std::setw(10) << std::setprecision(2) << c.gradePoints() << "\n";
    }

    std::cout << dash << "\n"
              << "Total credits       : " << std::setprecision(1) << sem.totalCredits << "\n"
              << "Total grade points  : " << std::setprecision(2) << sem.totalPoints << "\n"
              << "Semester GPA        : " << sem.gpa << "\n"
              << "Overall CGPA        : " << cgpa << "\n"
              << line << "\n";
}

// ---------- main ----------

int main() {
    std::cout << "===== CGPA CALCULATOR =====\n\n";

    int n = readInt("Enter the number of courses taken this semester: ", 1);

    std::vector<Course> courses;
    courses.reserve(n);

    for (int i = 1; i <= n; i++) {
        std::cout << "\nCourse " << i << ":\n";
        Course c;
        c.name = trim(readLine("  Course name: "));
        if (c.name.empty()) c.name = "Course " + std::to_string(i);

        readGrade("  Grade (e.g. A, B+, or 3.5): ", c.grade, c.points);
        c.credits = readDouble("  Credit hours: ", 0.5, 30.0);
        courses.push_back(c);
    }

    Summary sem = calculateGPA(courses);

    // Overall CGPA: include previous semesters if any
    double prevCredits = 0.0, prevCgpa = 0.0;
    std::string answer = trim(readLine("\nDo you have previous semester records? (y/n): "));
    if (!answer.empty() && std::tolower(static_cast<unsigned char>(answer[0])) == 'y') {
        prevCredits = readDouble("  Total credit hours completed so far: ", 0.0, 1000.0);
        prevCgpa = readDouble("  Your previous CGPA (0-4): ", 0.0, 4.0);
    }

    double cgpa = calculateCGPA(sem, prevCredits, prevCgpa);
    displayReport(courses, sem, cgpa);
    return 0;
}
