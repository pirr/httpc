FROM gcc:4.9

# Branch / Tag / Commit Hash of libuv
ENV LIBUV_VERSION 0c1fa696aa502eb749c2c4735005f41ba00a27b8
ENV HTTPC_HOST  "0.0.0.0"
ENV HTTPC_PORT 7050

RUN useradd -ms /bin/bash httpc
USER httpc
WORKDIR /home/httpc

# --- Fetch libuv sources
RUN git clone https://github.com/libuv/libuv.git && \
    cd ./libuv && \
    git checkout ${LIBUV_VERSION} && \
    /bin/sh autogen.sh && \
    ./configure && \
    make
    # make check

# --- Copy sources
COPY src/ ./src
COPY Makefile .
COPY build.sh .

USER root
RUN ["chmod", "+x", "./build.sh"]
USER httpc

ENTRYPOINT [ "./build.sh" ]