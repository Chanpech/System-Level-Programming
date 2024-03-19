all: hscript.c 
	gcc -o hscript hscript.c 

submission: hscript.c Makefile
	tar -czvf prog4.zip hscript.c Makefile readme

clean: 
	rm hscript 
	rm -rf out_dir