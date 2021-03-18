assembler: numberingSystems.o inputHandlers.o commands.o tableTools.o secondScan.o buildOutputFiles.o firstScan.o assembler.o 
	gcc -g -ansi -Wall -pedantic numberingSystems.o inputHandlers.o commands.o tableTools.o secondScan.o buildOutputFiles.o firstScan.o assembler.o -o assembler

assembler.o: assembler.c inputHandlers.h firstScan.h generals.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

firstScan.o: firstScan.c firstScan.h secondScan.h buildOutputFiles.h generals.h inputHandlers.h commands.h numberingSystems.h 
	gcc -c -ansi -Wall -pedantic firstScan.c -o firstScan.o

secondScan.o: secondScan.c secondScan.h firstScan.h generals.h inputHandlers.h
	gcc -c -ansi -Wall -pedantic secondScan.c -o secondScan.o

commands.o: commands.c commands.h tableTools.h
	gcc -c -ansi -Wall -pedantic commands.c -o commands.o

inputHandlers.o: inputHandlers.c inputHandlers.h
	gcc -c -ansi -Wall -pedantic inputHandlers.c -o inputHandlers.o

tableTools.o: tableTools.c tableTools.h
	gcc -c -ansi -Wall -pedantic tableTools.c -o tableTools.o

numberingSystems.o: numberingSystems.c numberingSystems.h
	gcc -c -ansi -Wall -pedantic numberingSystems.c -o numberingSystems.o

buildOutputFiles.o: buildOutputFiles.c buildOutputFiles.h inputHandlers.h
	gcc -c -ansi -Wall -pedantic buildOutputFiles.c -o buildOutputFiles.o
