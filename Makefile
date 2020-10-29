CC=gcc



Assignment1:
	$(CC) -o bin/$@ src/$@.c -g -lm -lreadline -I.
	chmod a+x bin/$@

Assignment2:
	$(CC) -o bin/$@ src/$@.c -g -I.
	chmod a+x bin/$@

Assignment4:
	$(CC) -o bin/$@ src/$@.c -g -I.
	chmod a+x bin/$@

libconttest:
	$(CC) -o bin/$@ src/$@.c -g -I.
	chmod a+x bin/$@

clean:
	rm bin/*
