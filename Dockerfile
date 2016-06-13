# ps4dev/ps4sdk
#
# This is intended as a vm alternative (interactive container)
# A volume container may be useful too (bring your own OS/compiler container)

FROM voidlinux/voidlinux

ENV PS4DEV=/home/ps4dev
ENV PS4SDK=$PS4DEV/ps4sdk

RUN \
    groupadd -r ps4dev && \
    useradd --create-home -d /home/ps4dev --gid ps4dev ps4dev

WORKDIR $PS4DEV
COPY . $PS4SDK

RUN \
    xbps-install -Sy xbps ; \
    xbps-install -Sy make clang git socat && \
    cd ps4sdk && \
    make && \
    chown -R ps4dev:ps4dev /home/ps4dev

USER ps4dev
