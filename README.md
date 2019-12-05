# Non-functional tests for Roc

[![Build Status](https://travis-ci.org/roc-project/roc-tests.svg?branch=master)](https://travis-ci.org/roc-project/roc-tests)

The scope if this repo is non-functional real-time tests that should run on real hardware, like:

* service quality tests
* latency tests
* performance tests (benchmarks)
* stress tests

## Dependencies

* C++17 compiler
* CMake >= 3.0.0
* Google Test >= 1.10 (downloaded automatically)
* Roc (shoould be installed system-wide)

## Build

```
mkdir build
cd build
cmake ..
make -j4
cd ..
```

## Run

```
./bin/roc-tests
```

## Format code

```
./scripts/format.sh
```

## Authors

See [here](https://github.com/roc-project/roc-tests/graphs/contributors).

## License

[MPL-2.0](LICENSE)
