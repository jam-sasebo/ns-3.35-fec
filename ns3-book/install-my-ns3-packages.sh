#!/bin/sh

echo "install my ns3 packages"

NS3_HOME=~/ns-allinone-3.24.1/ns-3.24.1
MY_PACKAGE_HOME=~/ns3-book-3.24.1/work
WSCRIPT=wscript-3.24.1

cp -rp $MY_PACKAGE_HOME/local   $NS3_HOME/.
cp -rp $MY_PACKAGE_HOME/wlan    $NS3_HOME/.
cp -r $MY_PACKAGE_HOME/local/rr-queue/src-queue $NS3_HOME/src/queue
mv $NS3_HOME/wscript $NS3_HOME/wscript-2.24.1-org
cp $MY_PACKAGE_HOME/wscript/wscript-3.24.1 $NS3_HOME/wscript

cp $MY_PACKAGE_HOME/wlan/exp06/src-mesh/wscript-3.24.1 $NS3_HOME/src/mesh/wscript
cp $MY_PACKAGE_HOME/wlan/exp06/src-mesh/helper/* $NS3_HOME/src/mesh/helper/.
cp $MY_PACKAGE_HOME/wlan/exp06/src-mesh/model/dot11s/hwmp-protocol.cc $NS3_HOME/src/mesh/model/dot11s/.
cp $MY_PACKAGE_HOME/wlan/exp06/src-mesh/model/dot11s/hwmp-protocol.h  $NS3_HOME/src/mesh/model/dot11s/.

cd $NS3_HOME
./waf distclean
./waf -d debug --enable-examples --enable-tests configure
./waf
