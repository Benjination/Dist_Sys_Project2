# Distributed Computation System - Project 2

A distributed computation system using Docker containers and MPI communication, implementing a 6-node architecture for mathematical operations.

## Project Overview

This system distributes mathematical computations (addition, subtraction, multiplication, division) across multiple nodes using MPI (Message Passing Interface) for communication. The system can operate in two modes: interactive input or CSV file processing.

## Architecture

### 6-Node System Design
1. **UI Node (Rank 0)** - User interface and task distribution
2. **Addition Node (Rank 1)** - Processes addition operations
3. **Subtraction Node (Rank 2)** - Processes subtraction operations  
4. **Multiplication Node (Rank 3)** - Processes multiplication operations
5. **Division Node (Rank 4)** - Processes division operations
6. **Display Node (Rank 5)** - Result aggregation and output

### Data Flow
```
User Input → UI Node → Computation Nodes → Display Node → File/Output
```

### Communication Protocol
- **MPI Messages**: Structured data transfer between nodes
- **Computation Requests**: UI → Computation Nodes
- **Computation Results**: Computation Nodes → Display Node
- **Completion Signals**: UI → Display Node

## Current Implementation Status

### ✅ Completed
- [x] System architecture design
- [x] Requirements specification (functional & non-functional)
- [x] UML class design
- [x] Display node implementation (`display.c`)
- [x] Test data generation (1000-line CSV file)
- [x] File handling logic for CSV input

### 🚧 In Progress
- [ ] UI node implementation
- [ ] Computation node implementation
- [ ] MPI communication protocols
- [ ] Error handling and validation

### 📋 Planned
- [ ] Docker containerization
- [ ] Integration testing
- [ ] Performance optimization
- [ ] Documentation completion

## File Structure

```
Project_2/
├── README.md
├── ThePlan.md                 # Detailed system specifications
├── Project_2.drawio          # UML diagrams
├── Code/
│   ├── display.c             # Display node implementation
│   └── test_data.csv         # 1000-line test dataset
└── Requirements/
    └── [Requirements docs]
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
- **Language**: C
- **Communication**: MPI (Message Passing Interface)
- **Containerization**: Docker
- **Platform**: Cross-platform (Linux/macOS/Windows)

## Usage

### CSV File Format
```csv
operand1,operand2,operation,
1,2,+,
6,2,-,
5,4,*,
6,3,/,
```

### Running the System
```bash
# With CSV file
./display test_data.csv

# Interactive mode
./display
```

## Development Setup

### Prerequisites
- GCC compiler
- MPI library (OpenMPI/MPICH)
- Docker (for containerization)

### Build Instructions
```bash
# Compile display node
gcc -o display display.c -lmpi

# Test with sample data
./display test_data.csv
```

## Output Format
```
========================================
DISTRIBUTED COMPUTATION RESULTS
========================================
Execution Time: 2026-01-31 10:30:45
Total Operations: 1000

ADDITION RESULTS (250 operations):
  1.0 + 2.0 = 3.0
  7.0 + 3.0 = 10.0
  ...

SUBTRACTION RESULTS (250 operations):
  6.0 - 2.0 = 4.0
  ...

MULTIPLICATION RESULTS (250 operations):
  5.0 * 4.0 = 20.0
  ...

DIVISION RESULTS (250 operations):
  6.0 / 3.0 = 2.0
  ...

========================================
SUMMARY:
- Successful Operations: 1000
- Failed Operations: 0
- Total Execution Time: 2.34 seconds
========================================
```

## Docker Deployment (Planned)

The system will be containerized with:
- Individual Dockerfiles for each node type
- Docker Compose orchestration
- MPI networking configuration
- Shared volumes for data exchange

## Contributing

This project is part of a Distributed Systems course (Spring 2026). 

### Development Phases
1. **Phase 1**: Core C implementation with MPI
2. **Phase 2**: Docker containerization
3. **Phase 3**: Integration testing and optimization
4. **Phase 4**: Performance evaluation and documentation

## License

Educational project for academic purposes.

## Contact

- Repository: [https://github.com/Benjination/Dist_Sys_Project2](https://github.com/Benjination/Dist_Sys_Project2)
- Course: Distributed Systems - Spring 2026

---

**Status**: 🚧 Active Development - Core implementation in progress