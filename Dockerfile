FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get -y install gcc libuuid1 uuid-dev pkg-config cmake git
RUN mkdir /json && cd /json
RUN git clone https://github.com/json-c/json-c.git
RUN cd json-c
RUN mkdir json-c-build && cd json-c-build && cmake ../json-c && make && make test && make install
ARG CACHEBUST
RUN git clone https://github.com/cptvladski/adb.git
RUN cd /adb && git clone https://github.com/petewarden/c_hashmap.git
RUN cd ./adb && make
RUN ldconfig
CMD /adb/server