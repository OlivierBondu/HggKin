CC        = g++
CCFLAGS   = -Wall -g
SOURCES   =
ROOTFLAGS = `root-config --cflags`
ROOTLIBS  = `root-config --libs --ldflags`
ROOFITLIBS = -lRooFit -lRooFitCore -lMinuit -lFoam
ROOSTATSLIBS = -lRooStats

all: kin_dist.exe compare_dist2.exe fit_dist2.exe proba_perso.exe splot_gengghbkg.exe splot_reco1D.exe splot_recogghbkg.exe

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

proba_perso.o: proba_perso.cc
	$(CC) $(CCFLAGS) $(ROOTFLAGS) -c proba_perso.cc -o proba_perso.o

proba_perso.exe: proba_perso.o setTDRStyle.o
	$(CC) $(ROOTLIBS) $(ROOFITLIBS) proba_perso.o setTDRStyle.o -o proba_perso.exe

splot_gengghbkg.o: splot_gengghbkg.cc
	$(CC) $(CCFLAGS) $(ROOTFLAGS) -c splot_gengghbkg.cc -o splot_gengghbkg.o

splot_gengghbkg.exe: splot_gengghbkg.o setTDRStyle.o
	$(CC) $(ROOTLIBS) $(ROOFITLIBS) $(ROOSTATSLIBS) splot_gengghbkg.o setTDRStyle.o -o splot_gengghbkg.exe

splot_reco1D.o: splot_reco1D.cc
	$(CC) $(CCFLAGS) $(ROOTFLAGS) -c splot_reco1D.cc -o splot_reco1D.o

splot_reco1D.exe: splot_reco1D.o setTDRStyle.o
	$(CC) $(ROOTLIBS) $(ROOFITLIBS) $(ROOSTATSLIBS) splot_reco1D.o setTDRStyle.o -o splot_reco1D.exe

splot_recogghbkg.o: splot_recogghbkg.cc
	$(CC) $(CCFLAGS) $(ROOTFLAGS) -c splot_recogghbkg.cc -o splot_recogghbkg.o

splot_recogghbkg.exe: splot_recogghbkg.o setTDRStyle.o
	$(CC) $(ROOTLIBS) $(ROOFITLIBS) $(ROOSTATSLIBS) splot_recogghbkg.o setTDRStyle.o -o splot_recogghbkg.exe




clean:
	rm *.exe *.o
