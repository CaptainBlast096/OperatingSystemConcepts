/*
 Author: Jaleel Rogers
 Date: 11/17/24
 Description: Different schedulers to simulate scheduling multiple procceses
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std; // Helps me be lazy

struct Process {
    string name; // Name of process
    int burstDuration; // Total CPU burst time required
    int arrivalTime; // Time at which the process arrives in the system
    int startTime = -1; // Time at which the proccess starts execution, set to -1 to indicate proccess hasn't started
    int endTime = 0;  // Time at which the process finishes execution
};

vector<Process> readProcesses(const string& fileName) {
    vector<Process> processes; // Stores processes
    ifstream file(fileName); // Opens the file

    if (!file.is_open()) { // Check if the file was opened sucesfully
        cerr << "Error opening file" << endl;
        return processes;
    }

    string line;
    while (getline(file, line)) { // Read each line of the file
        istringstream iss(line);
        Process process;

        if (!(iss >> process.name >> process.burstDuration >> process.arrivalTime)) { // Extracts information from line
            cerr << "Error reading line: " << line << endl;
            continue;
        }
        processes.push_back(process); // Adds process to the vector
    }
    file.close(); // Close the file
    return processes;
}

class Scheduler { // Base class for different scheduling algorithms

protected:
    vector<Process> processes;
    int currentTime = 0; // Tracks the current simulated time
    int totalWaitingTime = 0; // Total waiting time of all processes
    int totalResponseTime = 0; // Total response time of all procceses
    int completedProcesses = 0; // Keeps track of completed procceses

public:
    Scheduler(const vector<Process>& procs) : processes(procs) {} // Initializes the processes
    virtual void schedule() = 0; // Helps child classes to provide their own implementation

    void calculateMetrics(int cycleLimit) { // Calculates and displays performance metrics
        int completedInLimit = 0;
        for (const auto& process : processes) {
            if (process.endTime > 0 && process.endTime <= cycleLimit) {
                completedInLimit++;
            }
        }

        double avgWaitingTime = static_cast<double>(totalWaitingTime) / processes.size(); // Avg time in queue waiting
        double avgResponseTime = static_cast<double>(totalResponseTime) / processes.size(); // Avg time taken to start
        double throughput = static_cast<double>(completedInLimit) / cycleLimit; // Number of processes completed

        cout << "Average Waiting Time: " << avgWaitingTime << endl;
        cout << "Average Response Time: " << avgResponseTime << endl;
        cout << "Throughput: " << throughput <<"\n" << endl;
    }

    void displayGanttChart(const vector<string>& chartLines) const { // Displays the Gantt Chart's visual representation
        cout << "Gantt Chart" << endl;
        for (const auto& line : chartLines) {
            cout << line << endl;
        }
        cout << endl;
    }
};

class FifoScheduler : public Scheduler { // Implements the First-In-First-Out algorithm
public:
    using Scheduler::Scheduler;

    void schedule() override {
        cout << "\n" <<"FIFO Scheduler" << endl;

        vector<string> chartLines(processes.size()); // Initialize a vector to store the Gantt chart

        int index = 0; // Used to access elements within chartLines vector

        for (auto& process : processes) { // Fills timeline with "_"
            while (currentTime < process.arrivalTime) {
                chartLines[index] += "_";
                currentTime++;
            }

            for (int i = 0; i < process.burstDuration; i++) { // Simulates execution of the current process
                chartLines[index] += "#";
                currentTime++;
            }

            process.endTime = currentTime; // Set the process' end time to the current time

            // Calculate wait and response times
            int waitTime = process.endTime - process.arrivalTime - process.burstDuration;
            int responseTime = (process.startTime == -1) ? currentTime - process.arrivalTime : process.startTime;

            // Accumulate the total waiting and response times for calculating averages later
            totalWaitingTime += waitTime;
            totalResponseTime += responseTime;

            index++; // Moves to the next process in the Gantt chart
        }
        displayGanttChart(chartLines); // Display the Gantt chart
        calculateMetrics(10);  // Calculate metrics within the 10 cycles as specified
    }
};


class SjfScheduler : public Scheduler { // Shortest Job First (SJF) Scheduler
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

            if (shortestIndex == -1) {
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


class RoundRobinScheduler : public Scheduler { // Round Robin Scheduler
private:
    int quantum;

public:
    RoundRobinScheduler(const vector<Process>& procs, int q) : Scheduler(procs), quantum(q) {}

    void schedule() override {
        cout << "Round Robin Scheduler" << endl;
        vector<string> chartLines(processes.size());
        vector<int> remainingBurst(processes.size());

        for (int i = 0; i < processes.size(); i++) {
            remainingBurst[i] = processes[i].burstDuration;
        }
        while (completedProcesses < processes.size()) { // Continue scheduling unitl all processes are completed
            bool progressMade = false; // Flag to indicate if any process made progress in this round
            
            for (int i = 0; i < processes.size(); i++) { // Check if the process has arrived and still has remaining BT
                
                if (processes[i].arrivalTime <= currentTime && remainingBurst[i] > 0) {
                    int timeSlice = min(quantum, remainingBurst[i]);
                    remainingBurst[i] -= timeSlice;
                    progressMade = true;

                    // Running cycles
                    for (int j = 0; j < timeSlice; j++) {
                        chartLines[i] += "#";
                        currentTime++;
                    }

                    if (remainingBurst[i] == 0) {
                        processes[i].endTime = currentTime;
                        completedProcesses++;
                    }
                } else if (remainingBurst[i] > 0) {
                    chartLines[i] += "_";
                }
            }
            if (!progressMade) currentTime++;
        }
        displayGanttChart(chartLines);
        calculateMetrics(10);
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
