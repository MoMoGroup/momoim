#!/bin/bash
#rm -rf deb server-build client-build
mkdir -p deb server-build client-build

cd server-build
cmake ../../ -DCMAKE_BUILD_TYPE=Release -DMOMO_TARGET=Server
make -j8 -s
cp */*.so ../momo-server_0.0.1/opt/momo-server
chmod -x ../momo-server_0.0.1/opt/momo-server/*.so
cp server/server ../momo-server_0.0.1/opt/momo-server/server
cd ../client-build
cmake ../../ -DCMAKE_BUILD_TYPE=Release -DMOMO_TARGET=Client
make -j8 -s
cp */*.so ../momo-client_0.0.1/opt/momo
chmod -x ../momo-client_0.0.1/opt/momo/*.so
cp client/client ../momo-client_0.0.1/opt/momo/client
cd ../
fakeroot dpkg-deb -b momo-server_0.0.1 deb
fakeroot dpkg-deb -b momo-client_0.0.1 deb
