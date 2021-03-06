# Dockerfile for the DD4 MUD server.

FROM gcc:4.9 AS build
COPY server/src /dd4/src
WORKDIR /dd4/src
RUN make clean; \
    make LINK=-static

FROM busybox:glibc AS server
COPY --from=build /dd4/src/envy /dd4/bin/envy
RUN mkdir -p /dd4/external/area && \
    mkdir -p /dd4/external/player
WORKDIR /dd4/external/area
EXPOSE 8888
CMD ["/dd4/bin/envy"]
