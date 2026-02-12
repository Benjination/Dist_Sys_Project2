# Use Ubuntu as base image with MPI support
FROM ubuntu:22.04

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Install MPI and development tools
RUN apt-get update && apt-get install -y \
    build-essential \
    openmpi-bin \
    openmpi-common \
    libopenmpi-dev \
    openssh-server \
    openssh-client \
    && rm -rf /var/lib/apt/lists/*

# Configure SSH for MPI communication between containers
RUN mkdir /var/run/sshd
RUN echo 'root:distributed' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed -i 's/#PubkeyAuthentication yes/PubkeyAuthentication yes/' /etc/ssh/sshd_config
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# Generate SSH keys for passwordless communication
RUN ssh-keygen -t rsa -f /root/.ssh/id_rsa -N ''
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
RUN chmod 600 /root/.ssh/authorized_keys
RUN chmod 700 /root/.ssh

# Configure SSH client to skip host key verification
RUN echo "StrictHostKeyChecking no" >> /etc/ssh/ssh_config
RUN echo "UserKnownHostsFile /dev/null" >> /etc/ssh/ssh_config

# Create working directory
WORKDIR /app

# Copy C source files and test data
COPY user_Interface.c .
COPY computation.c .
COPY display.c .
COPY test_data.csv .
COPY mpi_launcher.sh .
COPY tags.h .

# Compile the C applications with MPI
RUN mpicc -o user_interface user_Interface.c
RUN mpicc -o computation computation.c  
RUN mpicc -o display display.c

# Copy hostfile creation script
COPY create-hostfile.sh .

# Make scripts executable
RUN chmod +x mpi_launcher.sh
RUN chmod +x create-hostfile.sh

# Create default hostfile (will be overridden by create-hostfile.sh at runtime)
RUN echo "node1 slots=1" > hostfile && \
    echo "node2 slots=1" >> hostfile && \
    echo "node3 slots=1" >> hostfile && \
    echo "node4 slots=1" >> hostfile && \
    echo "node5 slots=1" >> hostfile && \
    echo "node6 slots=1" >> hostfile

# Expose SSH port for MPI communication
EXPOSE 22

# Start SSH daemon and keep container running
CMD ["/usr/sbin/sshd", "-D"]