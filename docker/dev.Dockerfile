# Dockerfile for a dev environment containing gcc, gdb, etc, compatible with DD4 source code.
#
# Build the image:
#   docker build -f dev.Dockerfile -t dd4-dev .
#
# Run the container, exposing server on port 18888:
#   docker run --rm -it -v<local dd4 directory>:/dd4-dev -w /dd4-dev -p 18888:8888 dd4-dev bash
#
# See 'dev' and 'buid-dev' targets in the main Makefile.

FROM gcc:9.5
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get -y install gdb vim less screen
COPY ./docker/dev_content/.screenrc /root/
COPY ./docker/dev_content/.gdbinit /root/
EXPOSE 8888
CMD ["/bin/bash"]
