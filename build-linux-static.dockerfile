FROM alpine:3.19.1 AS build-stage

RUN apk update && apk add --no-cache binutils cmake make libstdc++ libgcc musl-dev gcc g++ pkgconfig autoconf automake libtool git
RUN apk add --no-cache mbedtls-dev mbedtls-static curl-dev curl-static zlib-static
RUN apk add --no-cache file vim
RUN wget -c https://curl.se/download/curl-8.7.1.tar.gz -O - | tar -xz -C /opt/
WORKDIR /opt/curl-8.7.1
RUN autoreconf -fi
RUN ./configure --with-mbedtls --disable-shared --disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smb --disable-smtp --disable-gopher --disable-sspi --disable-mqtt --disable-manual --disable-docs --disable-ntlm --disable-largefile --without-libidn2 --disable-tls-srp --disable-libcurl-option --disable-alt-svc --disable-headers-api --disable-verbose --disable-ares --disable-aws --disable-netrc --without-brotli --without-nghttp2
RUN make -j `nproc` install
WORKDIR /opt/cpuminer-scash
COPY . .
RUN ./autogen.sh
RUN LDFLAGS="-static -static-libgcc" ./configure
RUN make -j `nproc`
RUN strip -s minerd

FROM scratch AS export-stage
COPY --from=build-stage /opt/cpuminer-scash/minerd /
