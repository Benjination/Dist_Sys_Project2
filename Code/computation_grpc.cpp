#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "computation.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using computation::ComputationService;
using computation::OperationRequest;
using computation::OperationResponse;

class ComputationServiceImpl final : public ComputationService::Service {
    Status ProcessOperation(ServerContext* context, const OperationRequest* request,
                          OperationResponse* response) override {
        
        std::cout << "Computation node received: ID=" << request->request_id() 
                  << ", Op=" << request->operation_type() 
                  << ", " << request->operand1() << " and " << request->operand2() << std::endl;

        response->set_request_id(request->request_id());
        response->set_status(0);  // Success

        double result = 0.0;
        bool error = false;
        std::string error_msg = "";

        // Perform the operation
        switch(request->operation_type()) {
            case 1: // Addition
                result = request->operand1() + request->operand2();
                break;
            case 2: // Subtraction  
                result = request->operand1() - request->operand2();
                break;
            case 3: // Multiplication
                result = request->operand1() * request->operand2();
                break;
            case 4: // Division
                if (request->operand2() != 0.0) {
                    result = request->operand1() / request->operand2();
                } else {
                    error = true;
                    error_msg = "Division by zero";
                    response->set_status(1);  // Error
                }
                break;
            default:
                error = true;
                error_msg = "Invalid operation type";
                response->set_status(1);  // Error
                break;
        }

        if (!error) {
            response->set_result(result);
            std::cout << "Computation result: " << result << std::endl;
        } else {
            response->set_error_message(error_msg);
            std::cout << "Computation error: " << error_msg << std::endl;
        }

        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    ComputationServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Computation node listening on " << server_address << std::endl;
    
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}