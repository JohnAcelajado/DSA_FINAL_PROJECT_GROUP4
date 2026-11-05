#ifndef FLOOD_REPORT_H
#define FLOOD_REPORT_H
#include <string>
#include <cctype>

struct FloodReport {
    int id = 0;
    std::string street;
    int severity = 1;
    std::string dateTime;
    std::string status = "Pending";
};

inline std::string severityName(int s) {
    if (s == 1) return "Low";
    if (s == 2) return "Moderate";
    if (s == 3) return "High";
    return "Critical";
}

inline std::string toLowerStr(std::string text) {
    for (size_t i = 0; i < text.size(); i++) {
        text[i] = std::tolower((unsigned char)text[i]);
    }
    return text;
}

#endif