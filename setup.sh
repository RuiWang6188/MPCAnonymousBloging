#! /bin/bash

cd $HOME 

# prereq
apt update
apt install -y git wget build-essential
apt install -y libssl-dev libgmp3-dev cmake liblog4cpp5-dev zlib1g-dev

# boost installation
wget https://boostorg.jfrog.io/artifactory/main/release/1.71.0/source/boost_1_71_0.tar.bz2
tar -xf boost_1_71_0.tar.bz2
cd boost_1_71_0/
./bootstrap.sh --prefix=/usr && ./b2 stage threading=multi link=shared
./b2 install threading=multi link=shared

# build libscapi
cd $HOME
git clone https://github.com/cryptobiu/libscapi.git
cd libscapi
make