# 3rd part libraries

Its advised, but not necessary, to compile and link the following third party
libraries from source.

The following are libraries used in this project

- pistache
- json11
- mstch
- mariadb connector c
- googletest
- grpc
- openssl
- boost
- c4conf
- yaml-cpp
- GSL

## 3rd party library versions

The following are the versions used and build with this project:
- pistach - vanila main github development branch:
	https://github.com/pistacheio/pistache
- json11 - json11-1.0.0
- mstch - mstch-1.0.2
- mariadb connector c - mariadb-connector-c-3.2.5-src
- googletest - googletest-release-1.11.0
- grpc - grpc-1.43.0
- openssl - openssl-3.0.1
- boost - boost_1_78_0
- c4conf - c4conf-0.1.0-src
- yaml-cpp - yaml-cpp-yaml-cpp-0.7.0
- GSL - GSL-3.1.0

The simpler way to build grpc is the following:
```
git clone --recurse-submodules -b v1.42.0 https://github.com/grpc/grpc
cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
    ../..
make -j
make install
popd
```
