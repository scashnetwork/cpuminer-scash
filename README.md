# cpuminer-scash

This is a multi-threaded CPU miner for Scash, based on pooler's
fork of Jeff Garzik's reference cpuminer.

Scash uses RandomX as it's mining algorithm, as specified here: https://github.com/scash-project/sips/blob/main/scash-protocol-spec.md

The cpuminer depends on the following libraries:
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

### Running the miner

Help message and options:
```
./minerd -h
```

Example usage to mine on 4 cpu threads:
```
./minerd -o 127.0.0.1:8342 -O username:password -t 4 --coinbase-addr=YOUR_SCASH_ADDRESS
```

Use large memory pages and disable thread binding:
```
./minerd -o 127.0.0.1:8342 -O username:password -t 4 --coinbase-addr=YOUR_SCASH_ADDRESS --largepages --no-affinity
```

Use with a Stratum v1 mining pool and print out any network and debugging messages:
```
./minerd --url=stratum+tcps://pool.domain.com:1234 --user=checkyourpool --pass=checkyourpool -P -D
```

Benchmark 20000 hashes, on each of 4 miner threads (by default, will try number of processors):
```
./minerd --benchmark -t 4
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

### Build instructions for running in MSYS2

To build a native Windows application which must run in a MSYS2 terminal.

In MSYS2 terminal:
```
git clone https://github.com/scash-project/cpuminer-scash --recursive
cd cpuminer-scash
./autogen.sh
LIBCURL="-lcurl.dll" ./configure
make
```

### Build instructions for running in Windows

To build a native Windows application which can run in Terminal and PowerShell.

In MSYS2 terminal:

Build a static version of libcurl which uses Windows SSL (Schannel) instead of OpenSSL.
```
wget https://curl.se/download/curl-8.7.1.tar.gz
tar xf curl-8.7.1.tar.gz
cd curl-8_7_1
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

### Running the miner

Usage is same as shown above for Linux e.g.

Help message and options:
```
./minerd.exe -h
```

Example usage to mine on 4 cpu threads:
```
./minerd.exe -o 127.0.0.1:8342 -O username:password -t 4 --coinbase-addr=YOUR_SCASH_ADDRESS
```

## Legacy Readme

```
Architecture-specific notes:
	ARM:	No runtime CPU detection. The miner can take advantage
		of some instructions specific to ARMv5E and later processors,
		but the decision whether to use them is made at compile time,
		based on compiler-defined macros.
		To use NEON instructions, add "-mfpu=neon" to CFLAGS.
	PowerPC: No runtime CPU detection.
		To use AltiVec instructions, add "-maltivec" to CFLAGS.
	x86:	The miner checks for SSE2 instructions support at runtime,
		and uses them if they are available.
	x86-64:	The miner can take advantage of AVX, AVX2 and XOP instructions,
		but only if both the CPU and the operating system support them.
		    * Linux supports AVX starting from kernel version 2.6.30.
		    * FreeBSD supports AVX starting with 9.1-RELEASE.
		    * Mac OS X added AVX support in the 10.6.8 update.
		    * Windows supports AVX starting from Windows 7 SP1 and
		      Windows Server 2008 R2 SP1.
		The configure script outputs a warning if the assembler
		doesn't support some instruction sets. In that case, the miner
		can still be built, but unavailable optimizations are left off.
		The miner uses the VIA Padlock Hash Engine where available.

Usage instructions:  Run "minerd --help" to see options.

Connecting through a proxy:  Use the --proxy option.
To use a SOCKS proxy, add a socks4:// or socks5:// prefix to the proxy host.
Protocols socks4a and socks5h, allowing remote name resolving, are also
available since libcurl 7.18.0.
If no protocol is specified, the proxy is assumed to be a HTTP proxy.
When the --proxy option is not used, the program honors the http_proxy
and all_proxy environment variables.

Also many issues and FAQs are covered in the forum thread
dedicated to this program,
	https://bitcointalk.org/index.php?topic=55038.0
```