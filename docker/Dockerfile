FROM kosztadani/wireshark-plugin-env:0.1

COPY docker/build.sh /src/
COPY *.c *.h CMakeLists.txt /src/plugin/

RUN echo "Building plugin" \
 && /src/build.sh linux 3.6 \
 && /src/build.sh linux 4.0 \
 && /src/build.sh linux 4.2 \
 && /src/build.sh linux 4.4 \
 && /src/build.sh mingw 4.2 \
 && /src/build.sh mingw 4.4