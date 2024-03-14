#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

const char* hostFilePath = "C:\\Windows\\System32\\drivers\\etc\\hosts";

void blockDomains(const vector<string>& domains);
void unblockDomains();
bool isBlockedTime();
void waitUntilNextHour();

int main() {
    vector<string> domains = { "127.0.0.1 www.twitch.tv",
                               "127.0.0.1 www.youtube.com",
                               "127.0.0.1 www.twitter.com",
                               "127.0.0.1 www.espn.com" };

    while (true) {
        if (isBlockedTime()) {
            blockDomains(domains);
        }
        else {
            unblockDomains();
        }
        waitUntilNextHour(); // Wait until next hour to check again
    }

    return 0;
}

void blockDomains(const vector<string>& domains) {
    ofstream file(hostFilePath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open hosts file for writing." << endl;
        return;
    }

    for (const auto& domain : domains) {
        file << domain << "\n";
    }
}

void unblockDomains() {
    //Delete the appended websites (4 lines)
    fstream file(hostFilePath, ios::app);
    int linecount = 0;
    string line;

    while (getline(file, line)) {
        linecount++;
        if (linecount >= 24) {
            file << line << "\n";
        }
    }
    file.close();
}

bool isBlockedTime() {
    time_t now = time(nullptr);
    struct tm* localTime = localtime(&now);
    int hour = localTime->tm_hour;
    return (hour >= 19); // Block sites after 7 PM (19:00)
}

void waitUntilNextHour() {
    // Sleep until the next hour
    auto now = chrono::system_clock::now();
    auto nextHour = now + chrono::hours(1) - chrono::minutes(chrono::system_clock::now().tm_min);
    this_thread::sleep_until(nextHour);
}

