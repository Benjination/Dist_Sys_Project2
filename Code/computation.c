#include <stdio.h>
#include <stdlib.h>

// Function prototypes
void run();
void listen_for_requests();
void receive_computation_request();
void perform_addition();
void perform_subtraction();
void perform_multiplication();
void perform_division();
void validate_division();
void process_computation();
void send_result_to_display();
void log_computation();

struct ComputationResult {
    int operation_type;
    double operand1;
    double operand2;
    double result;
    int request_id;
    int status;
};

int main (int argc, char *argv[]) {
    run();


    return 0;
}

void run(){
    // Main execution loop for computation nodes
    listen_for_requests();
};

void listen_for_requests(){
    // Listens for incoming computation requests from display node
    while (1) {
        receive_computation_request();
    }
};

void receive_computation_request(){
    // Receives computation request via MPI
    // Populates the info structure with received data
    struct ComputationResult info;


};  

void perform_addition(struct ComputationResult info){
    // Performs addition operation
    info.result = info.operand1 + info.operand2;
    send_result_to_display(info);
};

void perform_subtraction(struct ComputationResult info)
{
    // Performs subtraction operation
    info.result = info.operand1 - info.operand2;
    send_result_to_display(info);
};

void perform_multiplication(struct ComputationResult info){
    // Performs multiplication operation
    info.result = info.operand1 * info.operand2;
    send_result_to_display(info);
};

void perform_division(struct ComputationResult info){
    // Validates division operation
    if (info.operand2 == 0) {
        printf("Error: Division by zero attempted.\n Information not sent to display.");
        return;
    }
    // Performs division operation
    info.result = info.operand1 / info.operand2;
    send_result_to_display(info);
};

void process_computation(struct ComputationResult info)
{
    // Processes the computation based on the request
     if (info.operation_type == 1) {
        perform_addition(info);
    } else if (info.operation_type == 2) {
        perform_subtraction(info);
    } else if (info.operation_type == 3) {
        perform_multiplication(info);
    } else if (info.operation_type == 4) {
        perform_division(info);
    }
};

void send_result_to_display(struct ComputationResult info){
    // Sends computation result back to display node
};

void log_computation(struct ComputationResult info){
    // Logs computation details for auditing
    printf("Logged computation: Request ID %d, Operation %d, Operands %.2f and %.2f, Result %.2f, Status %d\n",
           info.request_id, info.operation_type, info.operand1, info.operand2, info.result, info.status);
};