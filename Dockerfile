FROM ubuntu:22.04

RUN apt-get update -y
RUN apt-get install -y gcc build-essential
RUN apt-get install -y nodejs
RUN apt-get install -y npm
