#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

struct Process {
    string name;
    int burstDuration, arrivalTime;
};

vector<Process> readProcesses(const string& fileName) {
    vector<Process> processes;
    ifstream file(fileName);

    if (!file.is_open()) {
        cerr << "Error openinig file" << endl;
        return processes;
    }

    string line;
    while (getline(file,line)) {
        istringstream iss(line);
        Process process;

        if (!(iss >> process.name >> process.burstDuration >> process.arrivalTime)) {
            cerr << "Error reading line: " << line << endl;
            continue;
        }
        processes.push_back(process);
    }
    file.close();
    return processes;
}


int main() {
    cout << "Hello, World!" << endl;
    return 0;
}
