FROM ubuntu:precise
RUN apt-get -qq update && apt-get -qq install gcc make libecpg-dev libpcap-dev
ADD . /code
CMD bash
