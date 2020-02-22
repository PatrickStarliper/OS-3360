/*  Name: Patrick Starliper
    PSID: 1494676
    Section: M/W
    Date Due: 02/19/2020
    Description:
        We are simulating multiple processes going through execution
        by scheduling them to work with what cores are available
        as well as interacting with queues for hardware and users
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <queue>
#include <algorithm>
#include <vector>

using namespace std;



/* TO-DO LIST 
make inq then non-iq work
make ssd queue work
make TTY work
make and test tcs
remove proc list state string
Make modules
Add documentation to functions, etc. 
*/



//----------------STRUCTURES-------------------
struct Event{
    string operation = ""; // state of event
    int parameter = 0; // time taken
    int release_time = 0; // release time
    int arr_time = 0; // arrival time
    string pro_class = "non-inter"; // set in interactive or non-interactive queue, default non-interactive
};
struct Process
{
    string current_state = ""; // RUNNING, WAITING, BLOCKED, TERMINATED
    string process_class = "non_inter"; // set in interactive or non-interactive queue, default non-interactive
    int PID = 0; // process ID
    int Time = 0; // process arrival time (current)
    vector<Event> events; // list of all events for process
};
struct pid_sort{ // to help sort processes
    int pid = 0;
    int time = 0; // process arrival time
    string state = "";
    string pro_class = "non-inter";
    int flag = 0;
};

// ----------------FUNCTIONS-------------------
bool compareByTime(const pid_sort &a, const pid_sort &b){ 
    return a.time < b.time;
    // helps sort the vectors in process table by time least to greatest
}
bool structEmpty(const pid_sort &a){
    return a.flag == 1;
    // helps delete processes, inside vectors, that are marked for deletion
}
void arrive(int time, int seqnumber){}
void core_request(int how_long, int seqnumber, bool isinteract){}
void core_release(){}
void ssd_request(){}
void ssd_release(){}
void user_request(){}
void user_release(){}
void print_process_table(const vector<pid_sort> &v, const Process (&array)[100]){
    int flag = 0;
    cout << "Process Table:" << endl;
    for (int i = 0; i < v.size(); i++){
        int current = v[i].pid;
        if (array[current].current_state == "RUNNING" || array[current].current_state == "READY" || array[current].current_state == "BLOCKED" || array[current].current_state == "TERMINATED"){
            cout << "Process " << v[i].pid << " is " << array[current].current_state << "." << endl;
            flag = 1;
        }
    }
    if (flag == 0){
        cout << "There are no active processes." << endl;
    }
    cout << endl;
}
void printSummary(int totalTime = 0, int numProcComplete = 0, int numSSD = 0, double avgBusyCores = 0, double utilSSD = 0){
    // print summary
    cout << "SUMMARY:" << endl;
    cout << "Total elapsed time: " << totalTime << " ms" << endl;
    cout << "Number of processes that completed: " << numProcComplete << endl;
    cout << "Total number of SSD accesses: " << numSSD << endl;
    cout << "Average number of busy cores: " << avgBusyCores << endl;
    cout << "SSD utilization: " << utilSSD << endl;
    // SSD Utilization: total time SSD was active / total time
    // Avg # of busy cores: Each time a process gets a core we add the amount of requested core time to a totalCoreTimes counter. 
    // At the end of the simulation we divide this TotalCoreTimes by the simulation time to obtain the average number of busy cores.
}

//------------------START OF MAIN---------------------
int main(){

    // initialize variables
    int clock = 0; // global time - (for output)
    int numProcComplete = 0; // Number of processes completed - (for output)
    int numSSD = 0; // Number of SSD accesses - (for output)
    int numCores = 0; // Number of cores available for processes
    double avgBusyCores = 0; // Avg number of busy cores
    double utilSSD = 0; // utilization of SSD
    string ssd_status = "FREE";
    vector<int> inq; // interactive queue, processes just completed user interaction, has priority over NON_INQ
    vector<int> non_inq; // non-interactive queue, processes waiting for a core
    vector<int> ssd_q; // ssd queue, stores PIDs
    Process process_list[100] = {}; // process table
    vector<pid_sort> proc_list_nums; // stores positions for process list, shows which pid will go first (sorted)
    vector<Event> event_list; // running/ready/blocked/terminated events
    
    // take input and place into data structures
    string keyword;
    int input_num;
    int timeToStart = 0;
    int pid = 0;
    int process_counter = 0;
    while(cin >> keyword >> input_num){ // (keyword,time)
        if (keyword == "NCORES"){
            numCores = input_num;
        }
        else if (keyword == "START"){
            // make new process
            timeToStart = input_num;
        }
        else if (keyword == "PID"){
            process_counter++;
            Process new_process;
            new_process.Time = timeToStart;
            new_process.PID = input_num;
            pid = input_num;
            process_list[input_num] = new_process;
            // add to list of pids
            pid_sort new_pid;
            new_pid.time = timeToStart;
            new_pid.pid = input_num;
            proc_list_nums.push_back(new_pid);
            // add start to event list
            Event new_event;
            new_event.operation = "START";
            new_event.parameter = timeToStart;
            process_list[pid].events.push_back(new_event);
        }
        else if (keyword == "CORE" || keyword == "SSD" || keyword == "TTY"){
            Event new_event;
            new_event.operation = keyword;
            new_event.parameter = input_num;
            process_list[pid].events.push_back(new_event);
        }

    } // while

    // CHECKING IF PROPERLY PLACED INSIDE (ignore)
    //cout << process_list[12].PID << " " << process_list[12].Time << endl << " " << process_list[12].events[0].operation << " " << process_list[12].events[0].parameter << endl;
    //for (int i = 0; i < process_list[12].events.size(); i++){
    //    cout << process_list[12].events[i].operation << " " << process_list[12].events[i].parameter << endl;
    //}
    //sort(proc_list_nums.begin(),proc_list_nums.end(),compareByTime);
    /*
    cout << "proc_list array" << endl;
    for (int i = 0; i < proc_list_nums.size(); i++){
        cout << proc_list_nums[i].pid << " " << proc_list_nums[i].time << endl;
    }*/
    /*
    cout << process_counter << " processes" << endl << endl;
    cout << "start of event lists" << endl;
    for (int i = 0; i < proc_list_nums.size(); i++){
        int current = proc_list_nums[i].pid;
        cout << process_list[current].events[0].operation << " " << process_list[current].events[0].parameter << endl;
    }*/
    /*
    cout << "all of event lists" << endl;
    for (int i = 0; i < proc_list_nums.size(); i++){
        int current = proc_list_nums[i].pid;
        for (int j = 0; j < process_list[current].events.size(); j++){
            cout << process_list[current].events[j].operation << " " << process_list[current].events[j].parameter << endl;
        }
    }*/

    // schedule process starts`
    sort(proc_list_nums.begin(),proc_list_nums.end(),compareByTime);
    // loop through events
    ///*
    while (process_counter > 0){
        // check queues for entries
        if (!inq.empty()){

        }
        if (!non_inq.empty()){
            for (int i = 0; i < non_inq.size(); i++){
                if ()
            }
        }
        // check event list for ongoing events
        if (!event_list.empty()){
            // release events
            for (int j = 0; j < event_list.size(); j++){
                if (event_list[j].arr_time == clock){ // could be <= or ==
                    clock += event_list[j].release_time;
                }
            }
            while (!event_list.empty()){
                event_list.pop_back();
            }
        }
        // process ongoing events
        for (int i = 0; i < proc_list_nums.size(); i++){
            int current = proc_list_nums[i].pid;
            if (process_list[current].events.empty()){
                process_counter += -1;
                process_list[current].current_state = "TERMINATED";
                cout << "Process " << current << " terminates at time " << clock << " ms." << endl;
                print_process_table(proc_list_nums,process_list);
                proc_list_nums[i].flag = 1;
                proc_list_nums.erase(remove_if(proc_list_nums.begin(),proc_list_nums.end(),structEmpty),proc_list_nums.end());
                numProcComplete += 1;
            }
        }

        // sort process list
        sort(proc_list_nums.begin(),proc_list_nums.end(),compareByTime);
        // look at queues at the start of loop for anything to pop?

        for (int i = 0; i < proc_list_nums.size(); i++){ // assumes event list is always sorted based on clock time
            int current = proc_list_nums[i].pid;
            // keyword START
            if (process_list[current].events[0].operation == "START"){
                // print out process is starting and process table
                cout << "Process " << current << " starts at time " << process_list[current].events[0].parameter << " ms." << endl;
                print_process_table(proc_list_nums,process_list);   
                process_list[current].Time = process_list[current].events[0].parameter; // arrival time
                proc_list_nums[i].time = process_list[current].events[0].parameter; // arrival time
                process_list[current].events[0].arr_time = process_list[current].Time;
                process_list[current].events[0].release_time = clock + process_list[current].events[0].parameter;
                process_list[current].events.erase(process_list[current].events.begin()); // pops start from event
                // execute next event
                if (process_list[current].events[0].operation == "CORE"){
                    // request core
                    if (numCores > 0){
                        numCores += -1; // take a core
                        process_list[current].current_state = "RUNNING"; // change states to running
                        proc_list_nums[i].state = "RUNNING";
                        process_list[current].events[0].release_time = clock + process_list[current].events[0].parameter; // update release time
                        event_list.push_back(process_list[current].events[0]); // add current event to event list
                        // +++++++++CHECK++++++++++
                        cout << event_list[0].operation << " " << event_list[0].parameter << " " << event_list[0].arr_time << " " << event_list[0].release_time << endl;
                        // +++++++++CHECK++++++++++
                        process_list[current].events.erase(process_list[current].events.begin()); // pop core event
                    }
                    else{ // send to queues
                        if (process_list[current].current_state == "non-inter"){
                            non_inq.push_back(current); // add current process to non-interactive queue
                            process_list[current].current_state = "READY"; // change states to ready
                            proc_list_nums[i].state = "READY";
                        }
                        else{
                            inq.push_back(current); // add current process to interactive queue
                            process_list[current].current_state = "READY";
                            proc_list_nums[i].state = "READY";
                        }// inner else
                    } // else
                } // if next is core
            }
            else if (process_list[current].events[0].operation == "CORE"){
                // check if waiting for a core
                // ++++++++++CHECK+++++++++++++++++++++
                cout << "entered CORE" << endl;
                if (!inq.empty()){
                    if (inq[0] == current && numCores > 0){
                        if (clock == process_list[current].events[0].release_time){
                            // request a core
                            numCores += -1;
                            process_list[current].current_state = "RUNNING";
                            proc_list_nums[i].state = "RUNNING";
                            process_list[current].events[0].release_time = clock + process_list[current].events[0].parameter;
                            event_list.push_back(process_list[current].events[0]);
                            process_list[current].events.erase(process_list[current].events.begin());
                        }
                    }
                    else if (non_inq[0] == current && numCores > 0){
                        if (clock == process_list[current].events[0].release_time){
                            // request a core
                            numCores += -1;
                            process_list[current].current_state = "RUNNING";
                            proc_list_nums[i].state = "RUNNING";
                            process_list[current].events[0].release_time = clock + process_list[current].events[0].parameter;
                            event_list.push_back(process_list[current].events[0]);
                            process_list[current].events.erase(process_list[current].events.begin());
                        }
                    }
                }
                // not waiting for a core
                if (numCores > 0){
                    numCores += -1;
                    process_list[current].current_state = "RUNNING";
                    proc_list_nums[i].state = "RUNNING";
                    process_list[current].events[0].release_time = clock + process_list[current].events[0].parameter;
                    event_list.push_back(process_list[current].events[0]);
                    process_list[current].events.erase(process_list[current].events.begin());
                }
                else{
                        if (process_list[current].current_state == "non-inter"){
                            non_inq.push_back(current); // add current process to non-interactive queue
                            process_list[current].current_state = "READY"; // change states to ready
                            proc_list_nums[i].state = "READY";
                        }
                        else{
                            inq.push_back(current); // add current process to interactive queue
                            process_list[current].current_state = "READY";
                            proc_list_nums[i].state = "READY";
                        }// inner else
                    } // else
            } // else if CORE
            else if (process_list[current].events[0].operation == "SSD"){
                // check ssd queue
                if (!ssd_q.empty()){
                    if (ssd_q[0] == current && ssd_status == "FREE"){
                        if (clock == process_list[current].events[0].release_time){
                            ssd_status = "BUSY";
                            numSSD += 1; // add to number of SSD access
                            process_list[current].events[0].release_time = clock + process_list[current].events[0].parameter;
                            event_list.push_back(process_list[current].events[0]);   
                            process_list[current].events.erase(process_list[current].events.begin());
                            process_list[current].current_state = "BLOCKED";
                            proc_list_nums[i].state = "BLOCKED";
                        }
                    }
                }
                // not in SSD queue
                // ssd request
                if (ssd_status == "FREE"){
                    ssd_status = "BUSY";
                    numSSD += 1; // add to number of SSD access
                    process_list[current].events[0].release_time = clock + process_list[current].events[0].parameter;
                    event_list.push_back(process_list[current].events[0]);   
                    process_list[current].events.erase(process_list[current].events.begin());
                    process_list[current].current_state = "BLOCKED";
                    proc_list_nums[i].state = "BLOCKED";
                }
                else {
                    ssd_q.push_back(current);
                    process_list[current].current_state = "READY";
                    proc_list_nums[i].state = "READY";
                }
            } // else if SSD
            else if (process_list[current].events[0].operation == "TTY"){
                continue;
            } // else if TTY
            else {
                cout << "PANIC: No correct operation found." << endl;
            } // else
        }// for loop
    }// while    
    //*/

    // print results after completion
    printSummary(clock,numProcComplete,numSSD);

    return 0;
}
