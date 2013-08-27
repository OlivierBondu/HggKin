CC        = g++
CCFLAGS   = -Wall -g
SOURCES   =
ROOTFLAGS = `root-config --cflags`
ROOTLIBS  = `root-config --libs --ldflags`
ROOFITLIBS = -lRooFit -lRooFitCore -lMinuit -lFoam
ROOSTATSLIBS = -lRooStats

#all: kin_dist.exe compare_dist2.exe fit_dist2.exe splot_ctheta_genggh.exe
all:

kin_dist.exe: kin_dist.cc
	$(CC) $(CCFLAGS) $(ROOTFLAGS) $(ROOTLIBS) kin_dist.cc -o kin_dist.exe

compare_dist2.o: compare_dist2.cc
	$(CC) $(CCFLAGS) $(ROOTFLAGS) -c compare_dist2.cc -o compare_dist2.o 

setTDRStyle.o: setTDRStyle.C setTDRStyle.h
	$(CC) $(CCFLAGS) $(ROOTFLAGS) -c setTDRStyle.C -o setTDRStyle.o

compare_dist2.exe: compare_dist2.o setTDRStyle.o
	$(CC) $(ROOTLIBS) compare_dist2.o setTDRStyle.o -o compare_dist2.exe

fit_dist2.o: fit_dist2.cc
	$(CC) $(CCFLAGS) $(ROOTFLAGS) -c fit_dist2.cc -o fit_dist2.o

fit_dist2.exe: fit_dist2.o setTDRStyle.o
	$(CC) $(ROOTLIBS) $(ROOFITLIBS) fit_dist2.o setTDRStyle.o -o fit_dist2.exe

splot_ctheta_genggh.o: splot_ctheta_genggh.cc
	$(CC) $(CCFLAGS) $(ROOTFLAGS) -c splot_ctheta_genggh.cc -o splot_ctheta_genggh.o

splot_ctheta_genggh.exe: splot_ctheta_genggh.o setTDRStyle.o
	$(CC) $(ROOTLIBS) $(ROOFITLIBS) $(ROOSTATSLIBS) splot_ctheta_genggh.o setTDRStyle.o -o splot_ctheta_genggh.exe


clean:
	rm *.exe *.o
