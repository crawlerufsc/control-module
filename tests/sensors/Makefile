all: bin

bin:
	rm -rf build
	rm -rf haltest
	mkdir -p build
	cd build && cmake ..
	cd build && make -j$(nproc)
	mv build/sensortest .

clean:
	rm -rf build haltest

