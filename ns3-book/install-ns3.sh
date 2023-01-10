#!/bin/sh

# define follwing variables acoording to ns3 version
#----------------------------------------------------
NS3_PKGS=ns-allinone-3.24.1.tar.bz2
NS3_HOME=~/ns-allinone-3.24.1/ns-3.24.1
NETANIM=~/ns-allinone-3.24.1/netanim-3.106
#----------------------------------------------------

NS3_PACKAGES1="python python-dev python-pygccxml mercurial valgrind gsl-bin libgsl0-dev libgsl0ldbl"
NS3_PACKAGES2="flex bison libfl-dev tcpdump gnuplot-x11 wireshark uncrustify doxygen graphviz imagemagic"
NS3_PACKAGES3="texlive texlive-extra-utils texlive-latex-extra python-pygraphviz python-kiwi"
NS3_PACKAGES4="python-pygoocanvas libgoocanvas-dev libboost-signals-dev libboost-filesystem-dev"
NS3_PACKAGES5="python-setuptools libxslt-dev qt4-dev-tools"

for ns3_package in $NS3_PACKAGES1 $NS3_PACKAGES2 $NS3_PACKAGES3 $NS3_PACKAGES4 $NS3_PACKAGES5
do 
  if [ $(dpkg-query -W -f='${Status}' $ns3_package 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
    sudo apt-get -y install $ns3_package;
  else
    printf '%-50s %s\n' $ns3_package ":ok installed"
fi
done

cd ~ 
if [ ! -e ${NS3_PKGS} ] ; then
    echo download ${NS3_PKGS} to local home directory
    wget http://www.nsnam.org/release/${NS3_PKGS}
    tar xjf ${NS3_PKGS}
fi

echo "making ns3 packages !!!!!"
cd ${NS3_HOME}/../
./build.py --enable-examples --enable-tests

cd ${NS3_HOME}
./waf -d debug --enable-examples --enable-tests configure
./waf
#./waf -d debug --enable-examples --enable-tests --with-openflow=../openflow --with-brite=../BRITE configure

if [ ! -e ${NETANIM}/NetAnim ] ; then
    echo "making ${NETANIM}/NetAnim !!!!!!!!!!"
    cd ${NETANIM}
    qmake NetAnim.pro; make
    sudo ln -s ${NETANIM}/NetAnim /usr/local/bin/
    rehash
fi

