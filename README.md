# Syslog Viewer


Build dependency "libssh2" for Win64:

```
cd where/you/like/to/install
git clone https://github.com/libssh2/libssh2.git
cd libssh2
mkdir dll
cmake -DCRYPTO_BACKEND=WinCNG -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=./dll -DCMAKE_GENERATOR_PLATFORM=x64 --build .
cmake --build . --target install
```

Download dependency "Poco" for Win64
https://github.com/pocoproject/poco/releases
