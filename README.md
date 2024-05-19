# cpuminer-scash

Cpuminer-scash is a free, high performance, open source, cross platform CPU miner for Scash.

Cpuminer-scash has been tested on Linux, Windows, macOS and on Intel/AMD x86-64 and ARM64 processors.

Scash uses RandomX 1.2.1 as it's mining algorithm, as specified here: https://github.com/scash-project/sips/blob/main/scash-protocol-spec.md

## Features

Cpuminer supports:
- Solo mining with Scash node (RPC getblocktemplate)
- Mining pools (Stratum V1 protocol)
- Hiveon OS

## Download
- Binary releases: https://github.com/scash-project/cpuminer-scash/releases
- Build from source (recommended)

## Hiveon OS instructions

Create a new flight sheet, select 'Custom miner', click 'Setup miner config' and then enter the following:
- Miner name: `cpuminer_scash`
- Installation URL: Copy the URL of the latest hiveon binary in Github releases
- Hash algorithm: `----` (`randomx` and `randomscash` can also be selected)
- Pool URL: `stratum+tcps://ADDRESS:PORT` (obtain address and port from mining pool)
- Wallet and worker template: `%WAL%.%WORKER_NAME%`
- Pass: `x` (or empty)
- Extra config arguments: `--largepages --quiet --no-affinity --threads=N` (run `minerd -h` to see full list of options)

## Build dependencies

Cpuminer depends on the following libraries:
- libcurl, https://curl.se/libcurl/
- jansson, https://github.com/akheron/jansson (jansson is included locally)
- RandomX, https://github.com/scash-project/RandomX (RandomX is included as a Git submodule)

## Linux

To build for Ubuntu Linux (or WSL in Windows)

### Install dependencies
```
sudo apt update
sudo apt upgrade
sudo apt install autoconf pkg-config g++ make libcurl4-openssl-dev
```

### Build instructions
```
git clone https://github.com/scash-project/cpuminer-scash --recursive
cd cpuminer-scash
./autogen.sh
./configure
make
```

### Build static binary

Run the script `build-linux-static.sh` which uses Docker to build a static binary for Linux in the `out` folder.
```
./build-linux-static.sh
out/minerd --version
```

The steps to create a static binary with Alpine Linux and musl are outlined in the `build-linux-static.dockerfile`, with mbedtls used as the libcurl SSL backend.

Using glibc to create a static binary is not recommended: https://stackoverflow.com/questions/57476533/why-is-statically-linking-glibc-discouraged/57478728#57478728

### Usage

Help message and options:
```
./minerd -h
```

Solo mine on 4 cpu threads, connected to a local Scash node:
```
./minerd -o 127.0.0.1:8342 -O username:password -t 4 --coinbase-addr=YOUR_SCASH_ADDRESS
```

Solo mine using large memory pages and disable thread binding:
```
./minerd -o 127.0.0.1:8342 -O username:password -t 4 --coinbase-addr=YOUR_SCASH_ADDRESS --largepages --no-affinity
```

Mine at a pool using large memory pages and print out networking and debugging messages:
```
./minerd --url=stratum+tcps://pool.domain.com:1234 --user=checkyourpool --pass=checkyourpool --largepages -P -D
```

Benchmark 5000 hashes (default is 1000), on each of 4 miner threads (default will use number of processors):
```
./minerd --benchmark --nonces 5000 -t 4
```

## Windows

MSYS2 is required to create a native Windows executable.

### Install MSYS2

Download and run installer (https://www.msys2.org/)
* Launch the MSYS2 terminal running default UCRT64 environment.
* You should see 'UCRT64' in the terminal prompt.

### Install dependencies

In MSYS2 terminal:
```
pacman -S git autoconf pkgconf automake make mingw-w64-ucrt-x86_64-curl mingw-w64-ucrt-x86_64-gcc
```

### Build instructions (for running in MSYS2)

To build a native Windows application which must run in a MSYS2 terminal.

In MSYS2 terminal:
```
git clone https://github.com/scash-project/cpuminer-scash --recursive
cd cpuminer-scash
./autogen.sh
LIBCURL="-lcurl.dll" ./configure
make
```

### Build instructions (for running anywhere on Windows)

To build a native Windows application which can run in Terminal and PowerShell.

In MSYS2 terminal:

Build a static version of libcurl which uses Windows SSL (Schannel) instead of OpenSSL.
```
pacman -S libtool
wget https://curl.se/download/curl-8.7.1.tar.gz
tar xf curl-8.7.1.tar.gz
cd curl-8.7.1
autoreconf -fi
./configure --with-schannel --disable-shared --disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smb --disable-smtp --disable-gopher --disable-sspi --disable-mqtt --disable-manual --disable-docs --disable-ntlm --disable-largefile --without-libidn2 --disable-tls-srp --disable-libcurl-option --disable-alt-svc --disable-headers-api --disable-verbose --disable-ares --disable-aws --disable-netrc --without-brotli --without-nghttp2 --without-libpsl --without-zstd
make
make install
```

Now build the miner using this new static version of libcurl.
```
git clone https://github.com/scash-project/cpuminer-scash --recursive
cd cpuminer-scash
./autogen.sh
LIBCURL=`pkg-config --static --libs libcurl` LDFLAGS="-static -static-libgcc" ./configure CFLAGS="-DCURL_STATICLIB"
make
```

### Usage

Usage is same as shown above for Linux e.g.

Help message and options:
```
./minerd.exe -h
```

Solo mine connected to a local Scash node:
```
./minerd.exe -o 127.0.0.1:8342 -O username:password --coinbase-addr=YOUR_SCASH_ADDRESS
```

Mine at a pool using large memory pages:
```
./minerd.exe --url=stratum+tcps://pool.domain.com:1234 --user=checkyourpool --pass=checkyourpool --largepages
```
