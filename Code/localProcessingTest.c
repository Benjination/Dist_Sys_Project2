#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// Tag definitions (kept for compatibility, but not used in local processing)
#define ALU_TAG 0
#define COUNT_TAG 1
#define RESULT_TAG 2
#define TERMINATE_TAG 3

#define PRINTS_USER 0
#define PRINTS_COMP 0
#define PRINTS_DISP 0

// Synchronization variables
int expected_results = 0;     // How many results we expect from UI
int received_results = 0;     // How many results we've actually received (including errors)
int ui_done_signal = 0;       // Flag: UI has finished sending work

struct ComputationResult {
    int operation_type; //1:Addition, 2:Subtraction, 3:Multiplication, 4:Division
    double operand1;
    double operand2;
    double result;
    int request_id;
    int status;  // 0=success, 1=division_by_zero, 2=other_error
    double time;
};

// Function prototypes from user_Interface.c
void process_interactive_input(void);
int process_csv_file(char* filename);
int parse_csv_line(char* line, struct ComputationResult* info);
void send_computation_request(struct ComputationResult info);
void send_completion_signal(int total_operations);

// Function prototypes from computation.c
void run();
void listen_for_requests();
int receive_computation_request();
void perform_addition(struct ComputationResult info);
void perform_subtraction(struct ComputationResult info);
void perform_multiplication(struct ComputationResult info);
void perform_division(struct ComputationResult info);
void process_computation(struct ComputationResult info);
void send_result_to_display(struct ComputationResult info);

// Function prototypes from display.c
int run_display();
void initialize_output_file();
void listen_for_results();
void receive_computation_result();
void store_result(struct ComputationResult info);
void organize_results_by_type();
void receive_completion_signal();
void calculate_statistics(struct ComputationResult info, bool print);
void display_to_console();
void send_termination_signals();
void cleanup_output_file();

int main (int argc, char *argv[]) {
    // Local processing - no MPI initialization needed
    printf("Local processing system started\n");
    
    // Initialize display system first
    initialize_output_file();
    
    if (argc > 1) {
        // CSV file provided
        printf("CSV file provided: %s\n", argv[1]);
        process_csv_file(argv[1]);
    } else {
        // No CSV file provided
        printf("No CSV file provided - interactive mode\n");
        process_interactive_input();
    }
    
    // After processing all computations, display results
    display_to_console();
    
    return 0;
}

// ============================================================================
// USER INTERFACE FUNCTIONS (from user_Interface.c)
// ============================================================================

void process_interactive_input(){
    // Prompts user for operations and sends to computation functions
    struct ComputationResult info;
    info.request_id = 1;
    info.status = 0;
    char input[100] = {0};

    printf("Enter computations in the format: operand1,operand2,operation (+, -, *, /)\n");
    printf("Type 'exit' to quit interactive mode.\n");
    while (strncmp(input, "exit", 4) != 0) {
        printf("Enter computation or type 'exit': ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        // Remove the newline character from fgets
        input[strcspn(input, "\n")] = '\0';

        if (strncmp(input, "exit", 4) == 0) {
            break;  
        }
        if (parse_csv_line(input, &info) == 0) {
            send_computation_request(info);
            info.request_id++;
        } else {
            printf("Invalid input format. Please try again.\n");
        }
    }
    send_completion_signal(info.request_id - 1);
}

int process_csv_file(char* filename){
    // Reads CSV file and sends operations to computation functions
    struct ComputationResult info;
    info.request_id = 1;
    info.status = 0;
    int total_operations = 0;

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    char line[256];
    int line_number = 0;
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        if (parse_csv_line(line, &info) == 0) {
            send_computation_request(info);
            info.request_id++;
            total_operations++;
        } else {
            printf("Failed to parse line %d: %s", line_number, line);
        }
    }
    fclose(file);
#if PRINTS_USER
    printf("Total lines processed: %d, Operations sent: %d\n", line_number, total_operations);
#endif
    send_completion_signal(total_operations);
    return 0;
};

int parse_csv_line(char* line, struct ComputationResult* info){
    // Parses a single line from CSV file to extract computation data
    char operation_str[10];
    if (sscanf(line, "%lf,%lf,%[^,]", &info->operand1, &info->operand2, operation_str) != 3) {
        fprintf(stderr, "Error parsing line: %s\n", line);
        return -1;
    }
    
    if (strcmp(operation_str, "+") == 0) {
        info->operation_type = 1;
    } else if (strcmp(operation_str, "-") == 0) {
        info->operation_type = 2;
    } else if (strcmp(operation_str, "*") == 0) {
        info->operation_type = 3;
    } else if (strcmp(operation_str, "/") == 0) {
        info->operation_type = 4;
    } else {
        fprintf(stderr, "Unknown operation: %s\n", operation_str);
        return -1;
    }
    return 0;
};

void send_computation_request(struct ComputationResult info){
    // Sends computation request to computation functions (local processing)
    // Operation type is still updated to maintain similarity with distributed version
    int target_rank = info.operation_type;  // Keep this for compatibility
    
    printf("Processing request locally: ID=%d, Op=%d, %.2f %s %.2f\n", 
           info.request_id, info.operation_type, info.operand1, 
           (info.operation_type == 1) ? "+" : 
           (info.operation_type == 2) ? "-" : 
           (info.operation_type == 3) ? "*" : "/", 
           info.operand2);
    
    // Process the computation locally instead of sending via MPI
    process_computation(info);
};

void send_completion_signal(int total_operations){
#if PRINTS_USER
    // Notifies that all computations are complete (local processing)
    printf("All %d computations have been processed locally.\n", total_operations);
#endif
    // Set expected results for local processing instead of sending via MPI
    expected_results = total_operations;
    ui_done_signal = 1;
};

// ============================================================================
// COMPUTATION FUNCTIONS (from computation.c)
// ============================================================================

void run(){
    // Main execution loop for computation nodes (not used in local processing)
    listen_for_requests();
};

void listen_for_requests(){
    // Listens for incoming computation requests (not used in local processing)
    while (1) {
        int result = receive_computation_request();
        if (result == -1) { // Termination signal received
            break;
        }
    }
};

int receive_computation_request(){
    // Receives computation request (not used in local processing)
    return -1; // Always return termination for local processing
};

void perform_addition(struct ComputationResult info){
    clock_t start_time = clock();
    // Performs addition operation
    info.result = info.operand1 + info.operand2;
    info.status = 0;  // Success status
    clock_t end_time = clock();
    info.time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    send_result_to_display(info);
};

void perform_subtraction(struct ComputationResult info)
{
    clock_t start_time = clock();
    // Performs subtraction operation
    info.result = info.operand1 - info.operand2;
    info.status = 0;  // Success status
    clock_t end_time = clock();
    info.time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    send_result_to_display(info);
};

void perform_multiplication(struct ComputationResult info){
    clock_t start_time = clock();
    // Performs multiplication operation
    info.result = info.operand1 * info.operand2;
    info.status = 0;  // Success status
    clock_t end_time = clock();
    info.time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    send_result_to_display(info);
};

void perform_division(struct ComputationResult info){
    clock_t start_time = clock();
    // Validates and performs division operation
    // CRITICAL: Always send response to maintain synchronization with display node
    
    if (info.operand2 == 0) {
        printf("Error: Division by zero attempted (ID: %d).\n", info.request_id);
        info.result = 0.0;  // Set result to 0 for division by zero
        info.status = 1;    // Set error status (1 = division by zero)
    } else {
        // Performs division operation
        info.result = info.operand1 / info.operand2;
        info.status = 0;    // Set success status
    }
    
    clock_t end_time = clock();
    info.time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    // Always send result back, even for errors
    send_result_to_display(info);
};

void process_computation(struct ComputationResult info)
{
    // Processes the computation - all operations go to the same computation functions
    // but operation type is still updated to maintain similarity with distributed version
    if (info.operation_type == 1) {
        perform_addition(info);
    } else if (info.operation_type == 2) {
        perform_subtraction(info);
    } else if (info.operation_type == 3) {
        perform_multiplication(info);
    } else if (info.operation_type == 4) {
        perform_division(info);
    } else {
        printf("Unknown operation type: %d\n", info.operation_type);
        info.result = 0.0;
        info.status = 2;  // Error status
        send_result_to_display(info);
    }
};

void send_result_to_display(struct ComputationResult info){
    // Sends computation result to display functions (local processing)
    printf("Sending result to display: ID=%d, Result=%.2f, Status=%d\n",
           info.request_id, info.result, info.status);
           
    // Store result directly instead of sending via MPI
    received_results++;
    store_result(info);
};

// ============================================================================
// DISPLAY FUNCTIONS (from display.c)
// ============================================================================

int run_display(){
    // Main execution loop for display node (modified for local processing)
    initialize_output_file();
    listen_for_results();
    
    // After displaying all results, clean up for next run
    display_to_console();
    return 0;
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
    // Listens for incoming messages (not used in local processing)
    // In local processing, results are stored directly via send_result_to_display
    while (1) {
        // Exit condition: received all expected results AND UI is done
        if (ui_done_signal && received_results >= expected_results) {
            printf("All results received (%d/%d). Ready to display.\n", 
                   received_results, expected_results);
            break;
        }
        // In local processing, we don't need to actively receive
        // Results are stored directly through function calls
        break;
    }
};

void receive_computation_result(){
    // Receives computation results (not used in local processing)
    // Results are stored directly via send_result_to_display
    struct ComputationResult info;
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

    // Storing content directly to CSV file
    FILE *file = fopen("computation_results.csv", "a");
    if (file == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }
    // RequestID,Operand1,Operand2,Operation,Result,Status,Time
    fprintf(file, "%d,%.2f,%.2f,%d,%.2f,%d,%.6f\n",
        info.request_id, info.operand1, info.operand2, info.operation_type,
        info.result, info.status, info.time);
    fclose(file);
    
    calculate_statistics(info, 0);
};

void organize_results_by_type(){
    // Organizes stored results by operation type for output
};

void receive_completion_signal(){
    // Receives completion signal (handled directly in local processing)
    ui_done_signal = 1;
    printf("UI finished. Expecting %d total results.\n", expected_results);
};

void calculate_statistics(struct ComputationResult info, bool print){
    // Calculates statistics such as total computations and average time
    static int totalOp[5];
    static double avgTime;
    if(!print){
        totalOp[info.operation_type - 1]++;
        avgTime += info.time;
    } else {
        if (expected_results > 0) {
            avgTime /= expected_results;
        }
        // print the statistics at the end of the result file
        FILE *file = fopen("computation_results.csv", "a");
        if (file == NULL) {
            perror("Error opening output file");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "\n\nStatistics\nTotal Operations\nAddition: %d\nSubtraction: %d\nMultiplication: %d\nDivision: %d\nAverage Time: %.6f\n",totalOp[0],totalOp[1],totalOp[2],totalOp[3],avgTime);
        fclose(file);

        printf("Addition:\t\t%d\nSubtraction:\t\t%d\nMultiplication:\t%d\nDivision:\t\t%d\nAverage time per operation: %.6f\nProcess complete. See computation_results.csv for results\n",
                totalOp[0],totalOp[1],totalOp[2],totalOp[3],avgTime);
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
    
    // Send termination signals (not needed in local processing)
    send_termination_signals();
};

void send_termination_signals(){
    // Send termination signals (not needed in local processing)
    printf("Local processing complete. All computations finished.\n");
};