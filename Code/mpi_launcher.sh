#!/bin/bash

# MPI Launcher Script - runs the correct program based on MPI rank
# Rank 0: UI (user_interface)
# Ranks 1-4: Computation (computation) 
# Rank 5: Display (display)

# Get MPI rank
RANK=${OMPI_COMM_WORLD_RANK}

echo "Starting MPI process with rank: $RANK"

case $RANK in
    0)
        echo "Rank $RANK: Starting UI node"
        exec /app/user_interface "$@"
        ;;
    [1-4])
        echo "Rank $RANK: Starting computation node"
        exec /app/computation
        ;;
    5)
        echo "Rank $RANK: Starting display node"
        exec /app/display
        ;;
    *)
        echo "Error: Unexpected rank $RANK"
        exit 1
        ;;
esac