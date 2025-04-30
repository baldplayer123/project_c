FROM debian

# Copy the malware to a temporary location (optional)
COPY ./serv /malware
# Install required packages and set up locale
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        apt-utils \
        locales \
        gcc \
        nano \
        vim \
        ssh \
        net-tools \
    && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8 \
    && rm -rf /var/lib/apt/lists/*




# Start Bash
# ENTRYPOINT ["/bin/bash"]
