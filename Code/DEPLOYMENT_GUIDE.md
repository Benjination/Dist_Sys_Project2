# MPI Distributed System - Deployment Guide

## Single Machine Deployment

Run all 6 nodes on one machine:

```bash
# Pull latest image
docker pull bennythepooh/mpi-distributed-system

# Start all nodes
docker compose -f docker-compose-single.yml up -d

# Run the MPI program
docker compose -f docker-compose-single.yml exec node1 mpirun -np 6 --hostfile /app/hostfile --allow-run-as-root /app/mpi_launcher.sh test_data.csv

# Stop all nodes
docker compose -f docker-compose-single.yml down
```

## Multi-Machine Deployment

### Setup Instructions:

1. **Update IP addresses** in both machine config files:
   - Edit `docker-compose-machine1.yml` and `docker-compose-machine2.yml`
   - Replace `192.168.1.100` with Machine 1's actual IP address
   - Replace `192.168.1.101` with Machine 2's actual IP address

2. **Ensure network connectivity**:
   - Both machines must be on the same network
   - Ports 2201-2206 must be open for SSH communication
   - Test with `ping` between machines

### Machine 1 (Nodes 1-3):

```bash
# Pull latest image
docker pull bennythepooh/mpi-distributed-system

# Start nodes 1-3
docker compose -f docker-compose-machine1.yml up -d
```

### Machine 2 (Nodes 4-6):

```bash
# Pull latest image  
docker pull bennythepooh/mpi-distributed-system

# Start nodes 4-6
docker compose -f docker-compose-machine2.yml up -d
```

### Running the Program:

From Machine 1 (where node1 is running):

```bash
# Run the MPI program across both machines
docker compose -f docker-compose-machine1.yml exec node1 mpirun -np 6 --hostfile /app/hostfile --allow-run-as-root /app/mpi_launcher.sh test_data.csv
```

### Cleanup:

```bash
# Machine 1
docker compose -f docker-compose-machine1.yml down

# Machine 2  
docker compose -f docker-compose-machine2.yml down
```

## Architecture:

- **Node 1 (Rank 0)**: UI/Input processing
- **Nodes 2-5 (Ranks 1-4)**: Computation nodes (Addition, Subtraction, Multiplication, Division)
- **Node 6 (Rank 5)**: Display/Output processing

## Notes:

- Results are written to `./results/computation_results.csv` on the machine where node1 or node6 runs
- SSH warnings during startup are normal and can be ignored
- The hostfile is automatically generated based on deployment mode