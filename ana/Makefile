
PROGS = $(shell ls *.C | cut -d'.' -f1)

INCLUDES = -I${G4WORKDIR}/include -I$(shell root-config --incdir) 
DEPLIBS=$(shell root-config --libs) -lEG
CC	=	g++
COPTS	=	-fPIC -DLINUX -O0  -g $(shell root-config --cflags) 

all:    programs 

programs: $(PROGS)
	echo making $(PROGS)

$(PROGS): % : %.o $(G4WORKDIR)/build/libg4hpDict.so 
	$(CC) -Wall -o $@ $< $(DEPLIBS) -L$(G4WORKDIR)/build -lg4hpDict 

%.o: %.C
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<

clean: 
	-rm *.o $(PROGS) 

