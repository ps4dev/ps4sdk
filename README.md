# PS4 SDK

![](https://avatars0.githubusercontent.com/u/15625873?v=3&s=128)
**ps4sdk** is a modular open source SDK for the PS4 with userland and kernel support.

The SDK currently supports most of the standard C library, various FreeBSD 9.0 userland
 and kernel, as well as some SCE functions.

It is designed to be adaptable to new firmwares and entry points and new reverse engineered
functions can be integrated into the SDK, by adding headers, function signatures and their
names to the list of function stubs.

Currently, running user and kernel code on firmwares ~**1.76** is supported.

## Prerequisites
* A unix or linux system, VM or Container
	* for OSX you will need to build clang from source or use the docker images
	* for Windows, you may want to use docker or a VM
* `clang 3.7` or later
* `make`

## Building
```bash
cd ps4sdk
make
```
This will take some time, as it will build all libraries. In addition, if you are updating
from a **git pull**, `make clean && make` instead of `make` may be required (and is advised),
to ensure proper functionality.

*Alternatively*, if you are purely interested in using the library, you may want to
download the precompiled [releases](https://github.com/ps4dev/ps4sdk/releases) instead.

## Running code

To execute code, you will need to grab the [elf loader](https://github.com/ps4dev/elf-loader) and run it locally.
It will provide you with the means to send `ps4sdk` generated elf files
via TCP to your PS4.

## Compiling your own code
The sdk is a collection of static libraries and headers. Once you have the header files
and the static libraries build or downloaded, you can compile your own code against them.

However, since the SDK is modular, it provides several interchangeable libs which overlap
in functionality and must be linked in a specific order to shadow and complement each
other appropriately. It is strongly advised that you use an example `Makefile` from
[ps4sdk-examples](https://github.com/ps4dev/ps4sdk-examples), which also provides the appropriate build flags
to the compiler.

Should you decide to use a provided example Makefile, you will have to specify the
ps4sdk folder as an environment variable e.g. `export PS4SDK=/home/<user>/ps4sdk`.
The Makefile will then build any source files in the local project folder `source`
using the headers found in the folder `include`, just as if your code would be one
of the examples. `ps4-elf` and `ps4-lib` are handy targets to generate executables
or libs.

## Docker images
The docker image offer you a consistent development runtime, which is not dependent
on your local environment. Once you know how to interact with the containers, specifically
how to mount volumes (see [here](https://docs.docker.com/engine/reference/commandline/volume_create/) and [here](https://docs.docker.com/engine/userguide/containers/dockervolumes/)),
development will be as convenient as a local installation. With the benefit that you
do not have to build ps4sdk yourself. However, the image is a bit large due to the clang tool-chain.

```bash
# Make sure newest container is used
docker pull ps4dev/ps4sdk
# Run docker for interactive development
# See volumes/-v on how to mount your code into the container
docker run (-v ... ) -i -t ps4dev/ps4sdk bash
```

From there, you can clone the elf-loader and examples into your volume, build and use them.
A stand alone elf-loader container is also available (but currently a bit large):

```bash
# Make sure newest container is used
docker pull ps4dev/elf-loader
# Run the elf loader (listens on port 5350)
docker run -p 5350:5350 -d ps4dev/elf-loader
-> a411...774
# Stop elf loader
docker kill a411
```
