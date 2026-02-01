
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ComputationResult {
    int operation_type; //1:Addition, 2:Subtraction, 3:Multiplication, 4:Division
    double operand1;
    double operand2;
    double result;
    int request_id;
    int status;
};

// Function prototypes
void process_interactive_input(void);
int process_csv_file(char* filename);
int parse_csv_line(char* line, struct ComputationResult* info);
int distribute_computation(int operation_type, double operand1, double operand2, int request_id);
int send_computation_request(int target_rank, int operation_type, double operand1, double operand2, int request_id);
void send_completion_signal(int total_operations);

int main (int argc, char *argv[]) {
    if (argc > 1) {
        // CSV file provided
        printf("CSV file provided: %s\n", argv[1]);
        process_csv_file(argv[1]);
    } else {
        // No CSV file provided
        printf("No CSV file provided - interactive mode\n");
        process_interactive_input();
    }
    return 0;
}


void process_interactive_input(){
    // Prompts user for operations and sends to computation nodes
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
            distribute_computation(info.operation_type, info.operand1, info.operand2, info.request_id);
            info.request_id++;
        } else {
            printf("Invalid input format. Please try again.\n");
        }
    }
    send_completion_signal(info.request_id - 1);
    return 0;
}

int process_csv_file(char* filename){
    // Reads CSV file and sends operations to computation nodes
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
            distribute_computation(info.operation_type, info.operand1, info.operand2, info.request_id);
            info.request_id++;
            total_operations++;
        } else {
            printf("Failed to parse line %d: %s", line_number, line);
        }
    }
    fclose(file);
    printf("Total lines processed: %d, Operations sent: %d\n", line_number, total_operations);
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

int distribute_computation(int operation_type, double operand1, double operand2, int request_id){
    // Sends computation request to appropriate computation node
    printf("Distributing computation request ID %d: %lf, %lf, operation %d\n", request_id, operand1, operand2, operation_type);
    return 0;
};

int send_computation_request(int target_rank, int operation_type, double operand1, double operand2, int request_id){
    // Sends MPI message with computation request to specific node
    printf("Sending computation request ID %d to node %d\n", request_id, target_rank);
    return 0;
};

void send_completion_signal(int total_operations){
    // Notifies display node that all computations are complete
    printf("All %d computations have been sent. Notifying display node of completion.\n", total_operations);
};