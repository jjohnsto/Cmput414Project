containers:
	docker build -t 414-ogre .

build: containers
	docker run --rm --name ogre-build -v /tmp/build/:/root/src/dist/bin/ 414-ogre bash -c "cmake . && make"
	cp /tmp/build/OgreApp ./bin
