# Real-time tests for Roc Toolkit

[![Build](https://github.com/roc-streaming/rt-tests/workflows/build/badge.svg?branch=main)](https://github.com/roc-streaming/rt-tests/actions/workflows/build.yml?query=branch%3Amain) [![Matrix chat](https://matrix.to/img/matrix-badge.svg)](https://app.element.io/#/room/#roc-streaming:matrix.org)

About rt-tests
--------------

The scope if this repo is [non-functional](https://en.wikipedia.org/wiki/Non-functional_testing) real-time integration tests for [Roc Toolkit](https://github.com/roc-streaming/roc-toolkit) public API. These tests are supposed to run on real hardware and produce metrics to be checked for regressions. Some examples of non-functional tests are:

* service quality tests
* latency tests
* performance tests (benchmarks)
* stress tests
* tests invloving real network interfaces and audio devices

About Roc
---------

Key features of Roc Toolkit:

* real-time streaming with guaranteed latency;
* robust work on unreliable networks like Wi-Fi, due to use of Forward Erasure Correction codes;
* CD-quality audio;
* multiple profiles for different CPU and latency requirements;
* relying on open, standard protocols, like RTP and FECFRAME;
* interoperability with both Roc and third-party software.

Compatible Roc Toolkit senders and receivers include:

* [cross-platform command-line tools](https://roc-streaming.org/toolkit/docs/tools/command_line_tools.html)
* [modules for sound servers](https://roc-streaming.org/toolkit/docs/tools/sound_server_modules.html) (PulseAudio, PipeWire)
* [C library](https://roc-streaming.org/toolkit/docs/api.html) and [bindings for other languages](https://roc-streaming.org/toolkit/docs/api/bindings.html)
* [end-user apps](https://roc-streaming.org/toolkit/docs/tools/applications.html)

Requirements
------------

* Linux or macOS
* C++17 compiler
* CMake >= 3.0.0
* Google Test >= 1.10 (downloaded automatically)
* Roc Toolkit (downloaded automatically)

Instructions
------------

### Simple build
When using this method, CMake will automatically download and build dependencies (Roc Toolkit). Roc will be statically linked into the modules and there is no need to install it into the system.

First install build tools:

```
sudo apt install -y \
    g++ pkg-config scons ragel gengetopt \
      libuv1-dev libunwind-dev \
      libpulse-dev libsox-dev \
      libcpputest-dev libspeexdsp-dev \
      libtool intltool autoconf automake make cmake
```

Next build and run:

```
make
```

To specify **roc-toolkit** branch use cmake flag `ROC_TOOLKIT_BRANCH`

```
mkdir build && cd build
cmake -DROC_TOOLKIT_BRANCH=master .. && make
```
### Additional targets

You can accomplish these additional tasks using the following targets.

To clean working build directory:

```
make clean
```

To format code:

```
make fmt
```

### Advanced bulid

You can disable automatic downloading of roc-toolkit and build it manually.

Download, build and install Roc Toolkit into the system as described on [this page](https://roc-streaming.org/toolkit/docs/building/user_cookbook.html)

```
mkdir build && cd build
cmake -DDOWNLOAD_ROC=OFF .. && make
```
To provide custom path to roc-toolkit library and headers use flags
`ROC_INCLUDE_DIR` and `ROC_LIB_DIR`



Workflow
--------

The diagram below illustrates generic workflow of a test from this repo.

Explanations:

* test writes stream of audio frames to roc sender
* roc sender encodes audio into UDP packets and sends them to roc receiver
* optionally, test inserts a UDP proxy between sender and receiver, to alter packets somehow (e.g. introduce packet losses)
* roc receiver receives packets, repairs losses, adjusts clock, decodes audio from UDP packets, and produces stream of audio frames
* test reads stream of audio frames from roc receiver
* test compares original and retrieved audio stream and computes some metrics, depending on the test (e.g. latency or service quality)

![](./docs/workflow.drawio.png)

Authors
-------

See [here](https://github.com/roc-streaming/rt-tests/graphs/contributors).

License
-------

[MPL-2.0](LICENSE)
