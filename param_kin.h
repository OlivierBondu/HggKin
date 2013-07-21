// mass windows 
int const n_window=2;
int const windowval[n_window]={2,3};

// cos(theta*) cuts
int const n_cuttheta=4;
int const cuttheta[n_cuttheta]={200,375,550,750};

//Variables used in the study
int const n_kinvar=3;
char const *kinvarval[n_kinvar]={"mass","pt","costheta"};
char const *kinvartitle[n_kinvar]={"m_{#gamma#gamma}","p_{T}","|cos(#theta*)|"};
float const binning[n_kinvar][2]={{100,180},{0,200},{0,1}};
int const n_bins[n_kinvar]={160,200,200};


// number of cut based studies from 0 to n_suty in kinvarval
int const n_study=2;
