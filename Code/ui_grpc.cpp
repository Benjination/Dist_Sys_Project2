#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "computation.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using computation::ComputationService;
using computation::DisplayService;
using computation::OperationRequest;
using computation::OperationResponse;
using computation::CompletionRequest;
using computation::CompletionResponse;

class UIClient {
public:
    UIClient() {
        // Computation nodes
        computation_nodes.push_back(ComputationService::NewStub(
            grpc::CreateChannel("172.30.0.12:50051", grpc::InsecureChannelCredentials())));
        computation_nodes.push_back(ComputationService::NewStub(
            grpc::CreateChannel("172.30.0.13:50051", grpc::InsecureChannelCredentials())));
        computation_nodes.push_back(ComputationService::NewStub(
            grpc::CreateChannel("172.30.0.14:50051", grpc::InsecureChannelCredentials())));
        computation_nodes.push_back(ComputationService::NewStub(
            grpc::CreateChannel("172.30.0.15:50051", grpc::InsecureChannelCredentials())));

        // Display node
        display_stub = DisplayService::NewStub(
            grpc::CreateChannel("172.30.0.16:50052", grpc::InsecureChannelCredentials()));
    }

    bool ProcessOperation(int request_id, int op_type, double op1, double op2) {
        OperationRequest request;
        request.set_request_id(request_id);
        request.set_operation_type(op_type);
        request.set_operand1(op1);
        request.set_operand2(op2);

        OperationResponse response;
        ClientContext context;

        // Round-robin to computation nodes
        int node_index = (request_id - 1) % computation_nodes.size();
        
        Status status = computation_nodes[node_index]->ProcessOperation(&context, request, &response);
        
        if (status.ok()) {
            std::cout << "Operation " << request_id << " completed with result: " << response.result() << std::endl;
            // Forward result to display node
            return ForwardToDisplay(response.request_id(), response.result(), response.status());
        } else {
            std::cout << "RPC failed: " << status.error_message() << std::endl;
            return false;
        }
    }

    bool ForwardToDisplay(int request_id, double result, int status) {
        // Implementation would send to display node via gRPC
        // For now, just log
        std::cout << "Forwarding result " << result << " to display node" << std::endl;
        return true;
    }

    bool NotifyCompletion(int total_ops) {
        CompletionRequest request;
        request.set_total_operations(total_ops);
        
        CompletionResponse response;
        ClientContext context;
        
        Status status = display_stub->NotifyComplete(&context, request, &response);
        return status.ok();
    }

    void ProcessInteractive() {
        std::string input;
        int request_id = 1;
        
        std::cout << "\n=== Interactive Mode ===" << std::endl;
        std::cout << "Enter computations in format: operand1,operand2,operation (+, -, *, /)" << std::endl;
        std::cout << "Type 'exit' to quit" << std::endl;
        
        while (true) {
            std::cout << "\nEnter computation: ";
            std::getline(std::cin, input);
            
            if (input == "exit" || input == "quit") {
                std::cout << "Exiting interactive mode. Notifying completion." << std::endl;
                NotifyCompletion(request_id - 1);
                break;
            }
            
            double op1, op2;
            int op_type;
            
            if (ParseCSVLine(input, &op1, &op2, &op_type)) {
                if (ProcessOperation(request_id, op_type, op1, op2)) {
                    request_id++;
                }
            } else {
                std::cout << "Invalid format. Please use: operand1,operand2,operation" << std::endl;
                std::cout << "Example: 5,3,+ or 10,2,*" << std::endl;
            }
        }
    }

    int ProcessCSVFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return -1;
        }

        std::string line;
        int request_id = 1;
        int processed = 0;

        std::cout << "Processing CSV file: " << filename << std::endl;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            double op1, op2;
            int op_type;
            
            if (ParseCSVLine(line, &op1, &op2, &op_type)) {
                if (ProcessOperation(request_id, op_type, op1, op2)) {
                    processed++;
                }
                request_id++;
            }
        }

        file.close();
        
        std::cout << "Processed " << processed << " operations. Notifying completion." << std::endl;
        NotifyCompletion(processed);
        
        return processed;
    }

private:
    std::vector<std::unique_ptr<ComputationService::Stub>> computation_nodes;
    std::unique_ptr<DisplayService::Stub> display_stub;

    bool ParseCSVLine(const std::string& line, double* op1, double* op2, int* op_type) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() != 3) return false;

        try {
            *op1 = std::stod(tokens[0]);
            *op2 = std::stod(tokens[1]);
            
            std::string operation = tokens[2];
            if (operation == "+") *op_type = 1;
            else if (operation == "-") *op_type = 2;
            else if (operation == "*") *op_type = 3;
            else if (operation == "/") *op_type = 4;
            else return false;
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
};

int main(int argc, char** argv) {
    UIClient client;

    if (argc > 1) {
        // Process CSV file
        client.ProcessCSVFile(argv[1]);
    } else {
        // Interactive mode
        client.ProcessInteractive();
    }

    return 0;
}