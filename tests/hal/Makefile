all: bin

bin:
	rm -rf build
	rm -rf haltest
	mkdir -p build
	cd build && cmake ..
	cd build && make
	mv build/haltest .

clean:
	rm -rf build haltest

