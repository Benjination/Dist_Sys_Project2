#!/bin/bash

# Dynamic hostfile creation script
# Supports both single-machine and multi-machine deployment

HOSTFILE="/app/hostfile"

# Clear existing hostfile
> $HOSTFILE

if [ "$DEPLOYMENT_MODE" = "multi-machine" ]; then
    # Multi-machine mode: use actual IP addresses
    echo "Creating multi-machine hostfile..."
    echo "${MACHINE1_IP:-localhost} slots=3" >> $HOSTFILE
    echo "${MACHINE2_IP:-localhost} slots=3" >> $HOSTFILE
    echo "Multi-machine hostfile created:"
else
    # Single-machine mode: use container names (default)
    echo "Creating single-machine hostfile..."
    echo "node1 slots=1" >> $HOSTFILE
    echo "node2 slots=1" >> $HOSTFILE
    echo "node3 slots=1" >> $HOSTFILE
    echo "node4 slots=1" >> $HOSTFILE
    echo "node5 slots=1" >> $HOSTFILE
    echo "node6 slots=1" >> $HOSTFILE
    echo "Single-machine hostfile created:"
fi

cat $HOSTFILE