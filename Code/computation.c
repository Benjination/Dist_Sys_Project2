#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "tags.h"

struct ComputationResult {
    int operation_type;
    double operand1;
    double operand2;
    double result;
    int request_id;
    int status;
    double time;
};

// Function prototypes
void run();
void listen_for_requests();
int receive_computation_request();
void perform_addition(struct ComputationResult info);
void perform_subtraction(struct ComputationResult info);
void perform_multiplication(struct ComputationResult info);
void perform_division(struct ComputationResult info);
void validate_division();
void process_computation(struct ComputationResult info);
void send_result_to_display(struct ComputationResult info);
void log_computation(struct ComputationResult info);

int main (int argc, char *argv[]) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    printf("Computation node started - ready to process operations\n");
    
    run();
    
    // Clean up MPI
    MPI_Finalize();
    return 0;
}

void run(){
    // Main execution loop for computation nodes
    listen_for_requests();
};

void listen_for_requests(){
    // Listens for incoming computation requests from display node
    while (1) {
        int result = receive_computation_request();
        if (result == -1) { // Termination signal received
            break;
        }
    }
};

int receive_computation_request(){
    // Receives computation request via MPI from UI node (rank 0)
    // Populates the info structure with received data
    struct ComputationResult info;
    MPI_Status status;
    
    // Receive the computation request from UI node
    MPI_Recv(&info, sizeof(struct ComputationResult), MPI_BYTE, 
             MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
    // Check if it's a termination signal
    if (status.MPI_TAG == TERMINATE_TAG) {
        printf("Computation node received termination signal. Shutting down.\n");
        return -1;
    }
    
    // Log what we received for debugging
    printf("Computation node received: ID=%d, Op=%d, %.2f %s %.2f\n", 
           info.request_id, info.operation_type, info.operand1, 
           (info.operation_type == 1) ? "+" : 
           (info.operation_type == 2) ? "-" : 
           (info.operation_type == 3) ? "*" : "/", 
           info.operand2);
    
    // Process the computation
    process_computation(info);
    
    return 0;
};  

void perform_addition(struct ComputationResult info){
    info.time = -MPI_Wtime();
    // Performs addition operation
    info.result = info.operand1 + info.operand2;
    info.status = 0;  // Success status
    info.time += MPI_Wtime();
    send_result_to_display(info);
};

void perform_subtraction(struct ComputationResult info)
{
    info.time = -MPI_Wtime();
    // Performs subtraction operation
    info.result = info.operand1 - info.operand2;
    info.status = 0;  // Success status
    info.time += MPI_Wtime();
    send_result_to_display(info);
};

void perform_multiplication(struct ComputationResult info){
    info.time = -MPI_Wtime();
    // Performs multiplication operation
    info.result = info.operand1 * info.operand2;
    info.status = 0;  // Success status
    info.time += MPI_Wtime();
    send_result_to_display(info);
};

void perform_division(struct ComputationResult info){
    info.time = -MPI_Wtime();
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
    
    info.time += MPI_Wtime();
    // Always send result back, even for errors
    send_result_to_display(info);
};

void process_computation(struct ComputationResult info)
{
    // Processes the computation - since UI routes by type,
    // this node will only receive one operation type, but handle any type for flexibility
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
    // Sends computation result back to display node
    printf("Sending result back to display: ID=%d, Result=%.2f, Status=%d\n",
           info.request_id, info.result, info.status);
           
    MPI_Send(&info, sizeof(struct ComputationResult), MPI_BYTE, 
                               5, RESULT_TAG, MPI_COMM_WORLD);
};

void log_computation(struct ComputationResult info){
    // Logs computation details for auditing
    printf("Logged computation: Request ID %d, Operation %d, Operands %.2f and %.2f, Result %.2f, Status %d\n",
           info.request_id, info.operation_type, info.operand1, info.operand2, info.result, info.status);
};