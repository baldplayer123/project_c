FROM debian

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
        openssl \
    && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8 \
    && rm -rf /var/lib/apt/lists/*

# WORKDIR /server
COPY . ./server
WORKDIR /server
# RUN openssl req -x509 -newkey rsa:2048 -keyout private.key -out certificate.crt -days 365 -nodes
RUN openssl req -x509 -newkey rsa:2048 -keyout private.key -out certificate.crt -days 365 -nodes -subj "/C=/ST=/L=/O=/OU=/CN="



# Start Bash
# ENTRYPOINT ["/bin/bash"]
