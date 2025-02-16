#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <unordered_set>
#include <limits>
#include <string>
#include <fstream>

using namespace std;

class CafeConstants {
public:
    static const int NUM_MANAGERS = 3;
    static const int NUM_ZONES = 3;
    static const int TABLES_PER_ZONE = 3;
    static const int CAFE_OPEN_TIME = 9 * 60;
    static const int CAFE_CLOSE_TIME = 23 * 60;
    static const int HOUR_RATE = 200;
};

class Table {
public:
    int number;
    int zone;
    bool occupied;
    bool inUse;
};

class Check {
public:
    int startTime;
    int endTime;
    int zone;
    string managerName;
    int tableNumber;
    int numPeople;
    vector<string> orders;
    double payment;
};

class Manager {
public:
    string name;
    vector<Check> checks;
    double totalRevenue;
};

void printCheck(const Check& check) {
    cout << setfill('-') << setw(40) << "" << setfill(' ') << endl;
    cout << setw(20) << left << "| Guests arrived: " << setw(19) << right << check.numPeople << endl;
    cout << setw(20) << left << "| Hall: " << setw(19) << right << check.zone << endl;
    cout << setw(20) << left << "| Table: " << setw(19) << right << "Table " << check.tableNumber << endl;
    cout << "| Orders: ";
    for (const string& order : check.orders) {
        cout << order << " ";
    }
    cout << setw(18) << right << endl;

    cout << setw(20) << left << "| Start time: " << setw(18) << right << check.startTime / 60 << ":" << check.startTime % 60 << endl;

    int exitHours = check.endTime / 60 % 24;
    int exitMinutes = check.endTime % 60;
    cout << setw(20) << left << "| Exit time: " << setw(18) << right << exitHours << ":" << exitMinutes << endl;

    cout << setw(20) << left << "| Payment: " << setw(18) << right << check.payment << " rubles " << endl;
    cout << setfill('-') << setw(40) << "" << setfill(' ') << endl;
}

int main() {
    //----------------------����� � ����--------------------------------

    time_t t = time(0);
    struct tm now;
    localtime_s(&now, &t);
    char filename[80];
    strftime(filename, sizeof(filename), "%d.%m.%Y___%H-%M_check.txt", &now);


    // Create ofstream for writing to the file
    ofstream outputFile(filename);

    // Redirect cout to the file
    streambuf* coutbuf = cout.rdbuf(); // Save the original cout buffer
    cout.rdbuf(outputFile.rdbuf());    // Redirect cout to the file

//----------------------����� � ����--------------------------------

    srand(static_cast<unsigned int>(time(0)));

    vector<Manager> managers(CafeConstants::NUM_MANAGERS);
    vector<Table> tables(CafeConstants::NUM_ZONES * CafeConstants::TABLES_PER_ZONE);

    for (int i = 0; i < CafeConstants::NUM_MANAGERS; ++i) {
        managers[i].name = "Manager " + to_string(i + 1);
        managers[i].totalRevenue = 0;
    }

    for (int i = 0; i < CafeConstants::NUM_ZONES * CafeConstants::TABLES_PER_ZONE; ++i) {
        tables[i].zone = i / CafeConstants::TABLES_PER_ZONE + 1;
        tables[i].number = i % CafeConstants::TABLES_PER_ZONE + 1;
        tables[i].occupied = false;
        tables[i].inUse = false;
    }


    int currentTime = CafeConstants::CAFE_OPEN_TIME;
    int managerCalls[CafeConstants::NUM_MANAGERS] = { 0 };
    int tableUsage[CafeConstants::NUM_ZONES * CafeConstants::TABLES_PER_ZONE] = { 0 };
    double totalRevenue = 0;

    //-------------------------------

    // �����:

    // ������� 1
    int maxManagerGenerations = 0;
    string mostFrequentManager;

    // ������� 2
    int maxTableGenerations = 0;
    int mostFrequentTableNumber = 0;

    // ������� 3
    int numGroupsGenerated = rand() % 11 + 90;


    //-------------------------------

    unordered_set<int> usedTables;

    while (currentTime <= CafeConstants::CAFE_CLOSE_TIME) {
        int numGroups = rand() % 5 + 1;

        for (int group = 0; group < numGroups; ++group) {
            int numPeople = rand() % 8 + 1;
            int zone = rand() % CafeConstants::NUM_ZONES + 1;

            // ������ ��� �������� ��������� ��������
            vector<int> availableTables;

            for (int i = 0; i < CafeConstants::TABLES_PER_ZONE; ++i) {
                int tableNumber = (zone - 1) * CafeConstants::TABLES_PER_ZONE + i + 1;

                // �������� �� ����� �� ����
                if (!tables[tableNumber - 1].inUse && usedTables.find(tableNumber) == usedTables.end()) {
                    availableTables.push_back(tableNumber);
                }
            }

            if (!availableTables.empty()) {
                // �������� ������������ ��������� ���� 
                int randomTableIndex = rand() % availableTables.size();
                int selectedTableNumber = availableTables[randomTableIndex];

                int tableIndex = selectedTableNumber - 1;

                tables[tableIndex].inUse = true;
                usedTables.insert(selectedTableNumber);

                Check check;
                check.startTime = currentTime;
                check.numPeople = numPeople;
                check.zone = zone;
                check.tableNumber = selectedTableNumber;

                int numCoffees = rand() % 3 + 1;
                int numTeas = rand() % 3 + 1;
                int numCookies = rand() % 2 + 1;
                int numMarmalades = rand() % 2 + 1;

                for (int i = 0; i < numCoffees; ++i) {
                    check.orders.push_back("Coffee");
                }

                for (int i = 0; i < numTeas; ++i) {
                    check.orders.push_back("Tea");
                }

                for (int i = 0; i < numCookies; ++i) {
                    check.orders.push_back("Cookie");
                }

                for (int i = 0; i < numMarmalades; ++i) {
                    check.orders.push_back("Marmalade");
                }

                check.endTime = currentTime + (numCoffees + numTeas + numCookies + numMarmalades) * 15;
                check.payment = (check.endTime - check.startTime) / 60.0 * CafeConstants::HOUR_RATE * numPeople;

                int randomManagerIndex = rand() % CafeConstants::NUM_MANAGERS;
                check.managerName = managers[randomManagerIndex].name;
                managers[randomManagerIndex].checks.push_back(check);
                managers[randomManagerIndex].totalRevenue += check.payment; // ���������� � ������ ������ ���������
                totalRevenue += check.payment; // ���������� � ������ ������

                // ��������� ����� � ���������� ������� ���������.
                managerCalls[randomManagerIndex]++;
                tableUsage[selectedTableNumber - 1]++;

                // �������� ���������� ��������
                cout << "Time: " << currentTime / 60 << ":" << setw(2) << setfill('0') << currentTime % 60 << setfill(' ') << endl;
                cout << "Manager: " << managers[randomManagerIndex].name << endl;
                printCheck(check);

                tables[tableIndex].inUse = false;
                usedTables.erase(selectedTableNumber);
            }
        }
        int randomOffset = (rand() % 12) * 5 + 5;
        currentTime += randomOffset;
    }

    // ������� 1
    for (int i = 0; i < CafeConstants::NUM_MANAGERS; ++i) {
        if (managerCalls[i] > maxManagerGenerations) {
            maxManagerGenerations = managerCalls[i];
            mostFrequentManager = "Manager " + to_string(i + 1);
        }
    }

    // ������� 2
    for (int i = 0; i < CafeConstants::NUM_ZONES * CafeConstants::TABLES_PER_ZONE; ++i) {
        if (tableUsage[i] > maxTableGenerations) {
            maxTableGenerations = tableUsage[i];
            mostFrequentTableNumber = i + 1;
        }
    }

    //--------------------task1 & 2 end----------------
    //--------------------��� �������----------------

    cout << " " << endl;
    cout << " " << endl;
    cout << "+-------------------------------------------------------------------------+" << endl;
    cout << "|                              Dop. zadaniya                              |" << endl;
    cout << "+-------------------------------------------------------------------------+" << endl;
    cout << "| 1) The most frequent manager: " << mostFrequentManager << " generated " << maxManagerGenerations << " times." << endl;
    cout << "| 2) The most frequent table number: Table " << mostFrequentTableNumber << " was generated " << maxTableGenerations << " times." << endl;
    cout << "| 3) Total revenue: " << totalRevenue << " rubles." << endl;
    cout << "| 4) Number of groups generated: " << numGroupsGenerated << "." << endl;
    cout << "+-------------------------------------------------------------------------+" << endl;

    //----------------------����� � ����--------------------------------

            // Close the file and restore cout
    outputFile.close();
    cout.rdbuf(coutbuf); // Reset cout to the original buffer

//----------------------����� � ����--------------------------------

    return 0;
}