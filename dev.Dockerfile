# Dockerfile for a dev environment containing gcc, gdb, etc, compatible with DD4 source code.
#
# Build the image:
#   docker build -f dev.server.Dockerfile -t dd4-dev .
#
# Run the container:
#   docker run --rm -it -v<local dd4 directory>:/dd4-dev -w /dd4-dev -p 8888:8888 dd4-dev bash
#
# See 'dev' target in the main Makefile.

FROM gcc:4.9
RUN apt-get update && \
    apt-get -y install gdb
EXPOSE 8888
CMD ["/bin/bash"]
