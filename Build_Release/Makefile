.PHONY: all clean assets build start
.DEFAULT_GOAL := all

ifeq ($(OS),Linux)
	NUMPROC := $(shell grep -c ^processor /proc/cpuinfo)
else ifeq ($(OS),Darwin)
	NUMPROC := $(shell sysctl hw.ncpu | awk '{print $$2}')
endif

assets:
	cd ASSETS && zip -r ../ASSETS.ZIP *

clean: 
	rm -f ASSETS.ZIP

start:
	./openclaw

build:
	cd .. && mkdir -p build && cd build && cmake .. && make

all: clean assets build start
