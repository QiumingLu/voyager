CXX=g++
CXXFLAGS="-std=c++11 -pthread -I/usr/local/include -I/usr/include"
CXXOUTPUT="/tmp/skywalker_build_detect_platform-cxx.$$"

# Test whether Protobuf library is installed
$CXX $CXXFLAGS -x c++ - -o $CXXOUTPUT 2>/dev/null  <<EOF
    #include <google/protobuf/arena.h>
    int main() {}
EOF
  if [ "$?" != 0 ]; then
    wget https://github.com/google/protobuf/archive/v3.2.0.tar.gz
    tar zxvf v3.2.0.tar.gz
    cd protobuf-3.2.0
    ./autogen.sh
    ./configure --prefix=/usr
    make
    sudo make install
    cd ..
    rm -rf protobuf-3.2.0
    rm -rf v3.2.0.tar.gz
  fi
rm -f $CXXOUTPUT
