FROM ubuntu:24.04 

ENV DEBIAN_FRONTEND=noninteractive 

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    gdb \
    valgrind \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /campusguard


COPY . . 

CMD ["bash"]