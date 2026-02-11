#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "tags.h"

// Synchronization variables
int expected_results = 0;     // How many results we expect from UI
int received_results = 0;     // How many results we've actually received (including errors)
int ui_done_signal = 0;       // Flag: UI has finished sending work

// global variable store?


struct ComputationResult {
    int operation_type;
    double operand1;
    double operand2;
    double result;
    int request_id;
    int status;  // 0=success, 1=division_by_zero, 2=other_error
    double time;  
};

// Function prototypes
void run();
void initialize_output_file();
void listen_for_results();
void receive_computation_result();
void store_result(struct ComputationResult info);
void organize_results_by_type();
void receive_completion_signal();
void generate_output();
void calculate_statistics();
void display_to_console();
void cleanup_output_file();


int main (int argc, char *argv[]) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    run();
    
    // Clean up MPI
    MPI_Finalize();
    return 0;
}   

void run(){
    // Main execution loop for display node
    initialize_output_file();
    listen_for_results();
    
    // After displaying all results, clean up for next run
    display_to_console();
    cleanup_output_file();
};

void initialize_output_file(){
    // Sets up output file for storing results
    FILE *file = fopen("computation_results.csv", "w");
    if (file == NULL) {
        perror("Error creating output file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "RequestID,Operand1,Operand2,Operation,Result,Status,Time\n");
    fclose(file);
};

void listen_for_results(){  
    // Receive the expected results value
    receive_completion_signal();
    // Listens for incoming messages from both UI and computation nodes
    while (1) {
        // Check for messages from UI (completion signal + expected count)
        // Check for messages from computation nodes (results)
        
        // Exit condition: received all expected results AND UI is done
        if (ui_done_signal && received_results >= expected_results) {
            printf("All results received (%d/%d). Ready to display.\n", 
                   received_results, expected_results);
            break;
        }
        
        receive_computation_result();
    }
};

void receive_computation_result(){
    // Receives either:
    // 1. Computation result from computation nodes (ranks 1-4)
    // 2. Completion signal + count from UI node (rank 0)
    //
    // CRITICAL: Computation nodes MUST always send a response,
    // even for errors (division by zero, etc.) to maintain count accuracy
    
    struct ComputationResult info;
    
    // TODO: MPI_Recv to determine message source and type
    // If from computation node: store result and increment received_results
    // If from UI node: set expected_results and ui_done_signal = 1
    MPI_Recv(&info, sizeof(struct ComputationResult), MPI_BYTE, 
             MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, &status);
    // For now, placeholder logic:
    received_results++;
    store_result(info);
};

void store_result(struct ComputationResult info){
    // Stores received result in appropriate data structure
    // Handles both successful computations and error cases
    //
    // Status codes:
    // 0 = success
    // 1 = division by zero error  
    // 2 = other computation error 

    // TODO: Store in array or write directly to CSV file
    // Even errors get stored to maintain accurate count

    // Storing bulk of content dirctly to CSV file
    FILE *file = fopen("computation_results.csv", "a");
    if (file == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    // RequestID,Operand1,Operand2,Operation,Result,Status,Time
    fprintf(file, "%d,%.2f,%.2f,%d,%.2f,%d,%.2f\n",
        info.request_id, info.operand1, info.operand2, info.operation_type,
        info.result, info.status, info.time); // time needs to be adjusted to seconds
    fclose(file);  // Close 
    
    calculate_statistics(info, 0);
};

void organize_results_by_type(){
    // Organizes stored results by operation type for output
};

void receive_completion_signal(){
    // Receives completion signal from UI node (rank 0)
    // This tells us: 
    // 1. How many total results to expect
    // 2. UI is done distributing work
    
    // TODO: MPI_Recv from rank 0 to get expected_results count
    if(expected_results == 0) {
        MPI_Recv(&expected_results, 1, MPI_INT, 
             0, COUNT_TAG, MPI_COMM_WORLD, &status);
    }
    ui_done_signal = 1;
    printf("UI node finished. Expecting %d total results.\n", expected_results);
};

void generate_output(){
    // Generates output file with all results
    //FILE *file = fopen("computation_reults")
};

void calculate_statistics(struct ComputationResult info, bool print){
    // Calculates statistics such as total computations and average time
    static int totalOp[5];
    static double avgTime;
    if(!print){
        totalOp[info.operation_type - 1]++;
        avgTime += info.time;
    } else {
        avgTime /= expected_results;
        // print the statistics at the end of the result file
        FILE *file = fopen("computation_results.csv", "a");
        if (file == NULL) {
            perror("Error opening output file");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "\n\nStatistics\nTotal Operations\nAddition: %d\n
                        Subtraction: %d\nMultiplication: %d\nDivision: %d\n
                        Average Time: %.2f\n",
                        totalOp[0],totalOp[1],totalOp[2],totalOp[3],avgTime);
        fclose(file);  // Close

        printf("Addition:\t\t%d\n
                Subtraction:\t\t%d\n
                Multiplication:\t%d\n
                Divison:\t\t%d\n
                Average time per operation: %.2f\n
                Process complete. See computation_results.csv for results\n",
                totalOp[0],totalOp[1],totalOp[2],totalOp[3],avgTime)
    }
};

void cleanup_output_file(){
    // Clears the output file contents after displaying results
    // This ensures each run starts fresh without contamination from previous runs
    FILE *file = fopen("computation_results.csv", "w");
    if (file == NULL) {
        perror("Error cleaning output file");
        return;
    }
    fclose(file);  // Close empty file
    printf("Output file cleared for next run\n");
};

void display_to_console(){
    // Displays results and statistics to console
    struct ComputationResult info;
    calculate_statistics(info, 1);
};


