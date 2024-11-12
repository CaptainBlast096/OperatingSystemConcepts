#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

struct Process {
    string name;
    int burstDuration, arrivalTime;
    int startTime = -1;
    int endTime = 0;  // Track when process finishes
};

vector<Process> readProcesses(const string& fileName) {
    vector<Process> processes;
    ifstream file(fileName);

    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return processes;
    }

    string line;
    while (getline(file, line)) {
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

// Base Scheduler class
class Scheduler {
protected:
    vector<Process> processes;
    int currentTime = 0;
    int totalWaitingTime = 0;
    int totalResponseTime = 0;
    int completedProcesses = 0;

public:
    Scheduler(const vector<Process>& procs) : processes(procs) {}
    virtual void schedule() = 0;

    void calculateMetrics(int cycleLimit) {
    int completedInLimit = 0;
    for (const auto& process : processes) {
        if (process.endTime > 0 && process.endTime <= cycleLimit) {
            completedInLimit++;
        }
    }

    double avgWaitingTime = processes.empty() ? 0 : static_cast<double>(totalWaitingTime) / processes.size();
    double avgResponseTime = processes.empty() ? 0 : static_cast<double>(totalResponseTime) / processes.size();
    double throughput = cycleLimit > 0 ? static_cast<double>(completedInLimit) / cycleLimit : 0;

    cout << "Average Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Response Time: " << avgResponseTime << endl;
    cout << "Throughput: " << throughput <<"\n" << endl;
}


    void displayGanttChart(const vector<string>& chartLines) const {
        cout << "Gantt Chart" << endl;
        for (const auto& line : chartLines) {
            cout << line << endl;
        }
        cout << endl;
    }
};

// FIFO Scheduler
class FifoScheduler : public Scheduler {
public:
    using Scheduler::Scheduler;

    void schedule() override {
        cout << "FIFO Scheduler" << endl;
        vector<string> chartLines(processes.size());
        
        int idx = 0;
        for (auto& process : processes) {
            // Advance to the process arrival time, filling with waiting cycles
            while (currentTime < process.arrivalTime) {
                chartLines[idx] += "_";
                currentTime++;
            }

            // Running process cycles
            for (int i = 0; i < process.burstDuration; i++) {
                chartLines[idx] += "#";
                currentTime++;
            }

            // Calculate wait and response times
            process.endTime = currentTime;
            int waitTime = process.endTime - process.arrivalTime - process.burstDuration;
            int responseTime = (process.startTime == -1) ? currentTime - process.arrivalTime : process.startTime;

            totalWaitingTime += waitTime;
            totalResponseTime += responseTime;
            idx++;
        }
        
        displayGanttChart(chartLines);
        calculateMetrics(10);  // Calculate metrics within the 10 cycles as specified
    }
};

// Shortest Job First (SJF) Scheduler
class SjfScheduler : public Scheduler {
public:
    using Scheduler::Scheduler;

    void schedule() override {
        cout << "SJF Scheduler" << endl;
        vector<string> chartLines(processes.size());
        currentTime = 0;

        while (completedProcesses < processes.size()) {
            int shortestIndex = -1;
            int shortestBurst = 1000000000;

            // Find shortest available job
            for (int i = 0; i < processes.size(); i++) {
                if (processes[i].arrivalTime <= currentTime && processes[i].burstDuration > 0 &&
                    processes[i].burstDuration < shortestBurst) {
                    shortestBurst = processes[i].burstDuration;
                    shortestIndex = i;
                }
            }

            if (shortestIndex == -1) {  // No process ready, fill with waiting cycle
                for (auto& line : chartLines) line += "_";
                currentTime++;
                continue;
            }

            Process& process = processes[shortestIndex];
            process.startTime = (process.startTime == -1) ? currentTime : process.startTime;

            // Run process cycles
            for (int i = 0; i < process.burstDuration; i++) {
                chartLines[shortestIndex] += "#";
                currentTime++;
            }

            process.endTime = currentTime;
            totalWaitingTime += process.endTime - process.arrivalTime - process.burstDuration;
            totalResponseTime += process.startTime - process.arrivalTime;

            process.burstDuration = 0;  // Mark process as completed
            completedProcesses++;
        }

        displayGanttChart(chartLines);
        calculateMetrics(10);
    }
};

// Round Robin Scheduler
class RoundRobinScheduler : public Scheduler {
private:
    int quantum;

public:
    RoundRobinScheduler(const vector<Process>& procs, int q) : Scheduler(procs), quantum(q) {}

    void schedule() override {
        cout << "Round Robin Scheduler" << endl;
        vector<string> chartLines(processes.size());
        vector<int> remainingBurst(processes.size());
        vector<bool> hasStarted(processes.size(), false);

        for (int i = 0; i < processes.size(); i++) {
            remainingBurst[i] = processes[i].burstDuration;
        }

        currentTime = 0;  // Reset time for Round Robin Scheduler
        bool processActive = true;

        while (processActive) {
            processActive = false;  // Assume no progress unless a process is ready

            for (int i = 0; i < processes.size(); i++) {
                if (processes[i].arrivalTime <= currentTime && remainingBurst[i] > 0) {
                    processActive = true;

                    if (!hasStarted[i]) {  // Track first access as response time
                        processes[i].startTime = currentTime;
                        totalResponseTime += processes[i].startTime - processes[i].arrivalTime;
                        hasStarted[i] = true;
                    }

                    int timeSlice = min(quantum, remainingBurst[i]);
                    remainingBurst[i] -= timeSlice;

                    // Run process cycles
                    for (int j = 0; j < timeSlice; j++) {
                        chartLines[i] += "#";
                        currentTime++;
                    }

                    if (remainingBurst[i] == 0) {  // Process is completed
                        processes[i].endTime = currentTime;
                        completedProcesses++;
                        totalWaitingTime += processes[i].endTime - processes[i].arrivalTime - processes[i].burstDuration;
                    }
                } else if (remainingBurst[i] > 0) {  // Process is waiting
                    chartLines[i] += "_";
                }
            }

            if (!processActive) currentTime++;  // Advance time if no process can execute
        }

        displayGanttChart(chartLines);
        calculateMetrics(10);  // Adjust cycle limit if needed
    }
};


int main() {
    string fileName = "processes.txt";
    vector<Process> processes = readProcesses(fileName);

    cout << "Processes Loaded:" << endl;
    for (const auto& process : processes) {
        cout << "Process: " << process.name
             << ", Burst Duration: " << process.burstDuration
             << ", Arrival Time: " << process.arrivalTime << endl;
    }

    FifoScheduler fifoScheduler(processes);
    fifoScheduler.schedule();

    SjfScheduler sjfScheduler(processes);
    sjfScheduler.schedule();

    RoundRobinScheduler rrScheduler(processes, 1);
    rrScheduler.schedule();

    return 0;
}
