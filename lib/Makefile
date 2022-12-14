all: bin

bin:
	rm -rf build
	mkdir -p build
	cd build && cmake ..
	cd build && make $(nproc)

clean:
	rm -rf build

install:
	sudo cp build/libcrawler_control* /usr/lib
	sudo mkdir -p /usr/include/crawler
	sudo cp include/* /usr/include/crawler