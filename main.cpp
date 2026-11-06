#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include "FloodReport.h"
#include "DynamicArray.h"
#include "Queue.h"
#include "Stack.h"
#include "MergeSort.h"
#include "BinarySearch.h"
#include "LinearSearch.h"

const std::string FILE_NAME = "reports.txt";

// ---------- input helpers ----------
int readInt(const std::string& prompt, int low, int high) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) std::exit(0);

        std::istringstream iss(line);
        int value;
        char extra;
        if (iss >> value && !(iss >> extra) && value >= low && value <= high) {
            return value;
        }
        std::cout << "Invalid input. Enter a number from " << low << " to " << high << ".\n";
    }
}

std::string readText(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) std::exit(0);

        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] == '|') line[i] = ' ';   // '|' is used in the save file
        }
        size_t first = line.find_first_not_of(' ');
        if (first != std::string::npos) {
            return line.substr(first, line.find_last_not_of(' ') - first + 1);
        }
        std::cout << "Input cannot be empty.\n";
    }
}

std::string currentDateTime() {
    std::time_t now = std::time(nullptr);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::localtime(&now));
    return std::string(buffer);
}

void printReport(const FloodReport& r) {
    std::cout << "  ID: " << r.id << " | " << r.street
              << " | " << severityName(r.severity) << " (" << r.severity << ")"
              << " | " << r.dateTime << " | " << r.status << "\n";
}

// ---------- comparisons / keys ----------
bool higherSeverityFirst(const FloodReport& a, const FloodReport& b) {
    return a.severity > b.severity;
}
int getId(const FloodReport& r) { return r.id; }
std::string getStreetLower(const FloodReport& r) { return toLowerStr(r.street); }

// ---------- the system ----------
class FloodSystem {
private:
    DynamicArray<FloodReport> archive;   // all reports, in order of report ID
    qNode<int>* front = nullptr;         // queue of pending report IDs
    qNode<int>* back = nullptr;
    sNode<int>* top = nullptr;           // stack of reports marked as fixed
    int nextId = 101;

    int findIndex(int id) {
        return binarySearch(archive.data(), archive.size(), id, getId);
    }

    // take the report out of the queue using only queue operations
    void removeFromQueue(int id) {
        int n = 0;
        for (qNode<int>* c = front; c != nullptr; c = c->next) n++;
        for (int i = 0; i < n; i++) {
            int x = 0;
            dequeue(&front, &back, x);
            if (x != id) enqueue(&front, &back, x);
        }
    }

public:
    ~FloodSystem() {
        clearQueue(&front, &back);
        clearStack(&top);
    }

    void load() {
        std::ifstream in(FILE_NAME);
        if (!in) return;
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream iss(line);
            std::string idText, street, sevText, dateTime, status;
            if (!std::getline(iss, idText, '|') || !std::getline(iss, street, '|') ||
                !std::getline(iss, sevText, '|') || !std::getline(iss, dateTime, '|') ||
                !std::getline(iss, status)) continue;
            try {
                FloodReport r;
                r.id = std::stoi(idText);
                r.street = street;
                r.severity = std::stoi(sevText);
                r.dateTime = dateTime;
                r.status = status;
                archive.push_back(r);
                if (r.status == "Pending") enqueue(&front, &back, r.id);
                if (r.id >= nextId) nextId = r.id + 1;
            } catch (...) {}
        }
        std::cout << "Loaded " << archive.size() << " report(s) from " << FILE_NAME << ".\n";
    }

    void save() {
        std::ofstream out(FILE_NAME);
        for (int i = 0; i < archive.size(); i++) {
            FloodReport& r = archive[i];
            out << r.id << '|' << r.street << '|' << r.severity << '|'
                << r.dateTime << '|' << r.status << '\n';
        }
        std::cout << "Saved " << archive.size() << " report(s) to " << FILE_NAME << ".\n";
    }

    // 1. Add report (queue)
    void addReport() {
        FloodReport r;
        r.id = nextId++;
        r.street = readText("Street or block name: ");
        std::cout << "Severity: 1 = Low, 2 = Moderate, 3 = High, 4 = Critical\n";
        r.severity = readInt("Severity (1-4): ", 1, 4);
        r.dateTime = currentDateTime();
        r.status = "Pending";

        archive.push_back(r);
        enqueue(&front, &back, r.id);
        std::cout << "Report added to the queue:\n";
        printReport(r);
    }

    // 2. Search by report ID (binary search)
    void searchById() {
        if (archive.size() == 0) { std::cout << "No reports yet.\n"; return; }
        int id = readInt("Report ID to search: ", 1, 999999);
        int index = findIndex(id);
        if (index == -1) std::cout << "Report " << id << " was not found.\n";
        else { std::cout << "Report found:\n"; printReport(archive[index]); }
    }

    // 3. Search by street (linear search)
    void searchByStreet() {
        if (archive.size() == 0) { std::cout << "No reports yet.\n"; return; }
        std::string street = toLowerStr(readText("Street or block name to search: "));
        int* matches = new int[archive.size()];
        int found = linearSearchAll(archive.data(), archive.size(), street, getStreetLower, matches);
        if (found == 0) {
            std::cout << "No reports found for that street.\n";
        } else {
            std::cout << found << " report(s) found:\n";
            for (int i = 0; i < found; i++) printReport(archive[matches[i]]);
        }
        delete[] matches;
    }

    // 4. Sort by severity (merge sort) - sorts a copy so the archive stays in ID order
    void sortReports() {
        int n = archive.size();
        if (n == 0) { std::cout << "No reports yet.\n"; return; }
        FloodReport* copy = new FloodReport[n];
        for (int i = 0; i < n; i++) copy[i] = archive[i];

        mergeSort(copy, n, higherSeverityFirst);

        std::cout << "Reports sorted by severity (most serious first):\n";
        for (int i = 0; i < n; i++) printReport(copy[i]);
        delete[] copy;
    }

    // 5. Mark as fixed (stack push)
    void markFixed() {
        if (archive.size() == 0) { std::cout << "No reports yet.\n"; return; }
        int id = readInt("Report ID to mark as fixed: ", 1, 999999);
        int index = findIndex(id);
        if (index == -1) { std::cout << "Report " << id << " was not found.\n"; return; }
        if (archive[index].status == "Fixed") { std::cout << "That report is already fixed.\n"; return; }

        archive[index].status = "Fixed";
        removeFromQueue(id);
        push(&top, id);
        std::cout << "Report marked as fixed:\n";
        printReport(archive[index]);
    }

    // 6. Undo last fix (stack pop)
    void undoFix() {
        int id;
        if (!pop(&top, id)) { std::cout << "Nothing to undo.\n"; return; }
        int index = findIndex(id);
        archive[index].status = "Pending";
        enqueue(&front, &back, id);   // goes back to the end of the queue
        std::cout << "Undo done. Report is pending again (back of the queue):\n";
        printReport(archive[index]);
    }

    // 7. View and count reports
    void viewReports() {
        int n = archive.size();
        if (n == 0) { std::cout << "No reports yet.\n"; return; }

        std::cout << "\nAll reports (" << n << "):\n";
        for (int i = 0; i < n; i++) printReport(archive[i]);

        std::cout << "\nPending queue (first to be served on top):\n";
        if (isEmpty(front)) std::cout << "  (empty)\n";
        int pos = 1;
        for (qNode<int>* c = front; c != nullptr; c = c->next) {
            std::cout << "  " << pos++ << ". Report " << c->data << "\n";
        }

        std::cout << "\nCount per street:\n";
        for (int i = 0; i < n; i++) {
            std::string key = toLowerStr(archive[i].street);
            bool seen = false;
            for (int j = 0; j < i; j++) {
                if (toLowerStr(archive[j].street) == key) { seen = true; break; }
            }
            if (seen) continue;

            int total = 0, pending = 0, fixed = 0;
            for (int k = 0; k < n; k++) {
                if (toLowerStr(archive[k].street) == key) {
                    total++;
                    if (archive[k].status == "Pending") pending++; else fixed++;
                }
            }
            std::cout << "  " << archive[i].street << ": " << total << " total, "
                      << pending << " pending, " << fixed << " fixed\n";
        }
    }
};

int main() {
    std::cout << "=== Barangay Flood Report and Clogged Drainage Tracking System ===\n";
    FloodSystem system;
    system.load();

    int choice = 0;
    while (choice != 8) {
        std::cout << "\n----- MAIN MENU -----\n"
                  << "1. Add report\n"
                  << "2. Search by report ID\n"
                  << "3. Search by street\n"
                  << "4. Sort reports by severity\n"
                  << "5. Mark as fixed\n"
                  << "6. Undo last fix\n"
                  << "7. View and count reports\n"
                  << "8. Save and exit\n";
        choice = readInt("Enter choice (1-8): ", 1, 8);
        std::cout << "\n";

        if (choice == 1) system.addReport();
        else if (choice == 2) system.searchById();
        else if (choice == 3) system.searchByStreet();
        else if (choice == 4) system.sortReports();
        else if (choice == 5) system.markFixed();
        else if (choice == 6) system.undoFix();
        else if (choice == 7) system.viewReports();
        else system.save();
    }
    return 0;
}