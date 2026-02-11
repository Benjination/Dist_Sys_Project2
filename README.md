# Distributed Computation System - Project 2

A distributed computation system using Docker containers and MPI communication, implementing a 6-node architecture for mathematical operations.

## Project Overview

This system distributes mathematical computations (addition, subtraction, multiplication, division) across multiple nodes using MPI (Message Passing Interface) for communication. The system operates in two modes: interactive input or CSV file processing, with full Docker containerization and MPI-based distributed execution.

## Architecture

### 6-Node System Design
1. **UI Node (Rank 0)** - User interface, CSV processing, and task distribution
2. **Computation Node (Ranks 1-4)** - Process arithmetic operations (addition, subtraction, multiplication, division)
3. **Display Node (Rank 5)** - Result collection and output generation

### Rank Distribution
- **Rank 0**: `user_interface` - CSV parsing and work distribution
- **Ranks 1-4**: `computation` - Mathematical operation processing
- **Rank 5**: `display` - Result aggregation and output

### Data Flow
```
CSV File → UI Node (Rank 0) → Computation Nodes (Ranks 1-4) → Display Node (Rank 5) → Results
```

### Communication Protocol
- **MPI Messages**: Structured data transfer using `MPI_Send` and `MPI_Recv`
- **Computation Requests**: UI distributes work by operation type
- **Computation Results**: All results forwarded to display node
- **Docker Networking**: SSH-enabled containers for MPI communication

### MPI Launcher System
Uses `mpi_launcher.sh` to distribute programs by rank:
- Rank 0 → `user_interface` executable
- Ranks 1-4 → `computation` executable  
- Rank 5 → `display` executable

## Current Implementation Status

### ✅ Completed
- [x] System architecture design and implementation
- [x] Full MPI-based distributed system
- [x] Docker containerization with 6-node setup
- [x] UI node implementation (`user_Interface.c`) with CSV processing
- [x] Computation node implementation (`computation.c`) for all operations
- [x] Display node implementation (`display.c`) for result collection
- [x] MPI launcher script (`mpi_launcher.sh`) for rank distribution
- [x] Docker Compose orchestration with networking
- [x] SSH-enabled containers for MPI communication
- [x] Test data generation and processing
- [x] End-to-end system testing and validation

### 🎯 System Features
- [x] Interactive mode and CSV file processing
- [x] All four arithmetic operations (addition, subtraction, multiplication, division)
- [x] Error handling and validation
- [x] Professional output formatting
- [x] Scalable containerized architecture

## File Structure

```
Project_2/
├── README.md
├── ThePlan.md                    # Detailed system specifications
├── Project_2.drawio             # UML diagrams
└── Code/
    ├── user_Interface.c          # UI node - CSV processing and work distribution
    ├── computation.c             # Computation nodes - arithmetic operations  
    ├── display.c                 # Display node - result collection
    ├── mpi_launcher.sh           # MPI rank-based program launcher
    ├── test_data.csv             # Test dataset (configurable size)
    ├── Dockerfile                # Container configuration with OpenMPI
    ├── docker-compose.yml        # 6-node orchestration setup
    └── results/                  # Output directory (mounted volume)
```

## Requirements

### Functional Requirements
- Support multiple user-submitted computations or CSV file input
- Perform addition, subtraction, multiplication, and division operations
- Generate and display professional computation results
- Handle both interactive and file-based input modes

### Non-Functional Requirements
- **Performance**: Process up to 1000 operations within 5 seconds
- **Reliability**: Graceful handling of division by zero errors
- **Scalability**: Architecture supports adding new computation types
- **Usability**: Clear error messages for invalid inputs

### Technical Specifications
- **Language**: C with MPI (Message Passing Interface)
- **Communication**: OpenMPI with SSH networking
- **Containerization**: Docker with Docker Compose
- **Architecture**: 6-node distributed system
- **Platform**: Cross-platform (Linux/macOS/Windows with Docker)

## Usage

### Docker-Based Distributed System

#### Quick Start
```bash
# Build and start the 6-node distributed system
docker compose up --build -d

# Run computations with CSV file
docker compose exec node1 mpirun -np 6 --hostfile /app/hostfile --allow-run-as-root /app/mpi_launcher.sh test_data.csv

# Stop the system
docker compose down
```

### CSV File Format
```csv
operand1,operand2,operation,
1,2,+,
6,2,-,
5,4,*,
6,3,/,
```

### Local Development (Without Docker)
```bash
# Compile all programs
mpicc -o user_interface user_Interface.c
mpicc -o computation computation.c  
mpicc -o display display.c

# Run with MPI (requires MPI environment)
mpirun -np 6 ./mpi_launcher.sh test_data.csv
```

## Development Setup

### Prerequisites
- Docker and Docker Compose
- (Optional) GCC compiler and MPI library for local development

### Container Architecture
- **Base Image**: Ubuntu 22.04
- **MPI**: OpenMPI with SSH communication
- **Network**: Custom bridge network with static IPs
- **Volumes**: Shared results directory

### Build Instructions
```bash
# Build and run the distributed system
docker compose up --build -d

# Check container status
docker compose ps

# View logs from specific node
docker compose logs node1

# Execute commands in containers
docker compose exec node1 bash
```

### Development Workflow
1. Edit C source files locally
2. Rebuild containers: `docker compose up --build -d`
3. Test with: `docker compose exec node1 mpirun -np 6 --hostfile /app/hostfile --allow-run-as-root /app/mpi_launcher.sh test_data.csv`
4. View results and debug as needed

## Output Format

The system processes operations and displays results with execution details:

```
Starting MPI process with rank: 0
Rank 0: Starting UI node
CSV file provided: test_data.csv
Total lines processed: 10, Operations sent: 10
All 10 computations have been sent. Notifying display node of completion.

Starting MPI process with rank: 1
Rank 1: Starting computation node
Computation node received: ID=1, Op=1, 1.00 + 2.00
Sending result back to display: ID=1, Result=3.00, Status=0

Starting MPI process with rank: 5
Rank 5: Starting display node
Display node started - ready to collect results
```

### System Performance
- **Parallel Processing**: Operations distributed across 4 computation nodes
- **Real-time Results**: Results processed and collected as they complete
- **Scalable Architecture**: Easy to modify for different operation types or node counts

## Docker Architecture

The system uses a fully containerized approach:

### Container Configuration
- **6 Containers**: One for each MPI rank (node1-node6)
- **Ubuntu 22.04**: Base image with OpenMPI installation
- **SSH Communication**: Passwordless SSH between containers
- **Shared Networking**: Custom bridge network for MPI communication
- **Volume Mounting**: Results directory shared between containers

### MPI Integration
- **OpenMPI**: Full MPI environment in each container
- **Hostfile**: Automatic generation for 6-node setup
- **Rank Distribution**: Automatic program selection via launcher script
- **Communication**: SSH-based inter-container MPI messaging

### Scalability
The architecture supports easy scaling:
- Add more computation nodes by modifying docker-compose.yml
- Extend operation types in computation.c
- Customize CSV processing in user_Interface.c

## Contributing

This project is part of a Distributed Systems course (Spring 2026). 

### Current Status: ✅ **COMPLETE & OPERATIONAL**

The distributed system is fully implemented and tested with:
- Complete MPI-based communication
- Docker containerization with 6-node architecture
- CSV processing and interactive modes
- All arithmetic operations supported
- Professional error handling and validation

### System Capabilities
- **Performance**: Handles variable-size datasets efficiently
- **Reliability**: Robust error handling and graceful degradation
- **Scalability**: Easy to extend with additional operation types
- **Maintainability**: Clean C code structure with clear separation of concerns

## License

Educational project for academic purposes.

## Contact

- Repository: [https://github.com/Benjination/Dist_Sys_Project2](https://github.com/Benjination/Dist_Sys_Project2)
- Course: Distributed Systems - Spring 2026

---

**Status**: ✅ **COMPLETE** - Fully operational distributed MPI system with Docker containerization

**Last Updated**: February 2026