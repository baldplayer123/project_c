FROM alpine:latest

RUN apk update && apk add --no-cache openssh bash

RUN adduser -D sshvictims && \
    echo "sshvictims:1234" | chpasswd

RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config && \
    sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

RUN ssh-keygen -A

EXPOSE 22

CMD ["/usr/sbin/sshd","-D"]

