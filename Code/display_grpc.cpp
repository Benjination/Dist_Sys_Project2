#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "computation.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using computation::DisplayService;
using computation::ResultRequest;
using computation::ResultResponse;
using computation::CompletionRequest;
using computation::CompletionResponse;

class DisplayServiceImpl final : public DisplayService::Service {
public:
    DisplayServiceImpl() : total_expected(0), received_count(0) {
        results_file.open("/app/results/computation_results.csv");
        if (!results_file.is_open()) {
            std::cerr << "Failed to open results file!" << std::endl;
        }
    }

    ~DisplayServiceImpl() {
        if (results_file.is_open()) {
            results_file.close();
        }
    }

    Status SaveResult(ServerContext* context, const ResultRequest* request,
                     ResultResponse* response) override {
        
        received_count++;
        std::cout << "Display received result " << received_count << ": ID=" << request->request_id()
                  << ", Result=" << request->result() << ", Status=" << request->status() << std::endl;

        if (results_file.is_open()) {
            results_file << request->request_id() << "," << request->result() 
                        << "," << request->status() << std::endl;
            results_file.flush();
        }

        response->set_success(true);
        return Status::OK;
    }

    Status NotifyComplete(ServerContext* context, const CompletionRequest* request,
                         CompletionResponse* response) override {
        
        total_expected = request->total_operations();
        std::cout << "Display node notified: expecting " << total_expected << " total results" << std::endl;
        std::cout << "Currently received: " << received_count << " results" << std::endl;

        if (received_count >= total_expected) {
            std::cout << "All results received. Display complete." << std::endl;
            std::cout << "Results saved to computation_results.csv" << std::endl;
        }

        response->set_acknowledged(true);
        return Status::OK;
    }

private:
    std::ofstream results_file;
    int total_expected;
    int received_count;
};

void RunServer() {
    std::string server_address("0.0.0.0:50052");
    DisplayServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Display node listening on " << server_address << std::endl;
    
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}