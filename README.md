HggKin
======

0) Setup root with roofit
source ~/load_ROOT.sh

1) Create links to the root files trees / create plot directory
./do_links.sh

2) Compile
make

3) Create reduced trees
./kin_dist.exe

4) Run splot
./splot_ctheta_genggh.exe

