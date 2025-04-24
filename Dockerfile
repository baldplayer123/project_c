FROM debian

# Copy the malware to a temporary location (optional)
COPY ./ldpreload_malware /malware/

# Install required packages and set up locale
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        apt-utils \
        locales \
        gcc \
        nano \
        vim \
    && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8 \
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user
RUN useradd -m -s /bin/bash maluser

# Set root password (for debugging or su if needed)
RUN echo "root:1234" | chpasswd

# Move malware binary to user home, set ownership and permissions
COPY ./ldpreload_malware /home/maluser/ldpreload_malware
RUN chown maluser:maluser /home/maluser/ldpreload_malware && \
    chmod +x /home/maluser/ldpreload_malware

# Set working directory and switch to non-root user
USER maluser
WORKDIR /home/maluser

# Start Bash
ENTRYPOINT ["/bin/bash"]
