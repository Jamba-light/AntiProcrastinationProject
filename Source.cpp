#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

const char* hostFilePath = "C:\\Windows\\System32\\drivers\\etc\\hosts";
const char* backupFilePath = "C:\\Windows\\System32\\drivers\\etc\\hosts.backup";

void blockDomains(const vector<string>& domains);
void unblockDomains();
bool isBlockedTime();
void waitUntilNextBlockTime();

int main() {
    vector<string> domains = { "127.0.0.1 www.twitch.tv",
                               "127.0.0.1 www.youtube.com",
                               "127.0.0.1 www.twitter.com",
                               "127.0.0.1 www.espn.com" };

    // Make a backup of the hosts file
    ifstream src(hostFilePath, ios::binary);
    ofstream dest(backupFilePath, ios::binary);
    dest << src.rdbuf();

    while (true) {
        if (isBlockedTime()) {
            blockDomains(domains);
        }
        else {
            unblockDomains();
            waitUntilNextBlockTime();
        }
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
    ifstream backup(backupFilePath);
    ofstream file(hostFilePath);
    file << backup.rdbuf();
}

bool isBlockedTime() {
    time_t now;
    struct tm localTime;
    localtime_s(&localTime, &now);
    int hour = localTime.tm_hour;
    return (hour >= 23); // Block sites after 7 PM (19:00)
}

void waitUntilNextBlockTime() {
    auto now = chrono::system_clock::now();
    auto blockTime = chrono::system_clock::from_time_t(time(nullptr));
    blockTime += chrono::hours(24 - chrono::system_clock::from_time_t(time(nullptr)).time_since_epoch().count() % 24 + 7);
    this_thread::sleep_until(blockTime);
}