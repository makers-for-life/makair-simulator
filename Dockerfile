FROM debian:buster

RUN mkdir -p /srv/simulator

ADD ./build/ /srv/simulator/build/
ADD ./data/ /srv/simulator/data/
ADD ./makair-firmware/ /srv/simulator/makair-firmware/
ADD ./srcs/ /srv/simulator/srcs/
ADD ./includes/ /srv/simulator/includes/

RUN apt-get update

RUN apt-get install -y make g++ libserial-dev socat

WORKDIR /srv/simulator/build

RUN make clean
RUN make

CMD ["sh", "/srv/simulator/build/docker.sh"]