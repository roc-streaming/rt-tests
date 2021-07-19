NUM_CPU ?= `nproc --all 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 1`

all: run

.PHONY: build
build:
	mkdir -p build
	cd build && cmake ..
	cd build && make --no-print-directory -j$(NUM_CPU)

run: build
	./bin/rt-tests

clean:
	rm -rf bin build

fmt:
	find tests -type f -name '*.[hc]pp' | xargs clang-format -verbose -i
