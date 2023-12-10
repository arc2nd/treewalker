# start with ubuntu
FROM ubuntu:22.04

# update and install some stuff
RUN apt-get clean && apt-get -y update
RUN apt-get install -y wget build-essential g++ python3-dev autotools-dev libicu-dev libbz2-dev libboost-all-dev

# get the boost tar
WORKDIR /srv/boost_install
RUN wget -O boost_1_83_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.83.0/boost_1.83_0.tar.gz/download
RUN tar xzvf boost_1_83_0.tar.gz
WORKDIR /srv/boost_install/boost_1_83_0/
RUN ./bootstrap.sh --prefix=/usr/
RUN ./b2
RUN ./b2/install

# cop over our work files
COPY . /srv/src
WORKDIR /srv/src

CMD echo "Boost Installed"




