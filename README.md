# HLS Proxy

A simple HLS proxy made in C++ from scratch.

### How to build

Before building make sure to install the following packages:
```shell script
sudo apt-get install build-essential cmake
```

Then simply launch the following command from a terminal:
```shell script
cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" /path/to/repo/HLSProxy && make
```

And finally launch any server using (for example):
```shell script
./hls-proxy-ex1 http://bitdash-a.akamaihd.net
./hls-proxy-ex2 bitdash-a.akamaihd.net & vlc http://127.0.0.1:8080/content/sintel/hls/playlist.m3u8 && fg
./hls-proxy-ex3 http://bitdash-a.akamaihd.net:80
```

The proxy server is listening on the port 8080 (see proxy.h).

## Built With

* [CLion 2019.2](https://www.jetbrains.com/clion/) - C/C++ IDE by JetBrains
* [CMake 3.10.2](https://cmake.org/) - A C++ building tool
* [Ubuntu 18.04.3 LTS](https://ubuntu.com/) - A linux operating system
* [VLC 3.0.7](https://www.videolan.org/vlc/index.html) - A multimedia player
