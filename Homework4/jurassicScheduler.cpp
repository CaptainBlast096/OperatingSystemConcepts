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

void fifoScheduling(const vector<Process>& processes) {
    //Increments as time passes
    int currentTime = 0;

    //Waiting time of all procceses
    int totalWaitingTime = 0;

    //Response time of all procceses
    int totalResponseTime = 0;

    //Amount of procceses completed
    int completedProcesses = 0;

    // Gantt Chart
    cout << "Gantt Chart" << endl;
    for (const auto& process : processes) {
        cout << process.name << " ";
    }
    cout << endl;

    // Loops until all processes are completed
    while (completedProcesses < processes.size()) {
        int nextProccessIndex = -1;
        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].arrivalTime <= currentTime && processes[i].burstDuration > 0) {
                nextProccessIndex = i;
                break;
            }
        }
        // If no process is ready, wait until the next arrival
        if (nextProccessIndex == -1) {
            cout << "_";
            currentTime++;
            continue;
        }

        cout << "#";
        processes[nextProccessIndex].burstDuration--;

        int waitingTime = currentTime - processes[nextProccessIndex].arrivalTime;
        totalWaitingTime += waitingTime;

        int responseTime = currentTime - processes[nextProccessIndex].arrivalTime;
        totalResponseTime += responseTime;

        //Check for process completion
        if (processes[nextProccessIndex].burstDuration == 0) {
            completedProcesses++;
        }
        currentTime++;
    }
    cout << endl;

    double avgWaitingTime = static_cast<double>(totalWaitingTime) / processes.size();
    double avgResponseTime = static_cast<double>(totalResponseTime) / processes.size();
    cout << "Average Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Response Time: " << avgResponseTime << endl;

    int totalExecutionTime = currentTime;
    double throughput = static_cast<double>(processes.size()) / totalExecutionTime;
    cout << "Throughput (for reference): " << throughput << endl;
}

int main() {
    string fileName = "processes.txt";
    vector<Process> processes = readProcesses(fileName);

    for (const auto& process : processes) {
        cout << "Process: " << process.name
             << "Burst Duration: " << process.burstDuration
             << ", Arrival Time: " << process.arrivalTime << endl;
    }
    return 0;
}
