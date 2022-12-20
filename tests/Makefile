all: bin

bin:
	rm -rf build
	rm -rf unittest
	mkdir -p build
	cd build && cmake ..
	cd build && make -j$(nproc)
	mv build/unittest .
	

test: bin
	rm -rf build
	./unittest

clean:
	rm -rf build unittest

