#ifndef __prepfold_cmd__
#define __prepfold_cmd__
/*****
  command line parser interface 
                        -- generated by clig (Version: 1.0.1)

  The command line parser `clig':
  (C) 1995 Harald Kirsch (kir@iitb.fhg.de)
*****/

#ifndef FALSE
#  define FALSE (1==0)
#  define TRUE  (!FALSE)
#endif

typedef struct s_Cmdline {
  /***** -pkmb: Raw data in Parkes Multibeam format */
  char pkmbP;
  /***** -ebpp: Raw data in Efflesberg-Berkeley Pulsar Processor format */
  char ebppP;
  /***** -nobary: Do not barycenter the data */
  char nobaryP;
  /***** -DE405: Use the DE405 ephemeris for barycentering instead of DE200 (the default) */
  char de405P;
  /***** -p: The folding period (s) */
  char pP;
  double p;
  int pC;
  /***** -pd: The folding period derivative (s/s) */
  char pdotP;
  double pdot;
  int pdotC;
  /***** -f: The folding frequency (hz) */
  char freqP;
  double freq;
  int freqC;
  /***** -fd: The folding frequency derivative (hz/s) */
  char dfdtP;
  double dfdt;
  int dfdtC;
  /***** -n: The number of bins in the profile.  Defaults to the number of sampling bins which correspond to one folded period */
  char proflenP;
  int proflen;
  int proflenC;
  /***** -psr: Name of pulsar to fold (do not include J or B) */
  char psrnameP;
  char* psrname;
  int psrnameC;
  /***** -rzwcand: The candidate number to fold from 'infile'_rzw.cand */
  char rzwcandP;
  int rzwcand;
  int rzwcandC;
  /***** -rzwfile: Name of the rzw search file to use (include the full name of the file) */
  char rzwfileP;
  char* rzwfile;
  int rzwfileC;
  /***** -bincand: Fold a binary pulsar but take the input data from this candidate number in 'infile'_bin.cand */
  char bincandP;
  int bincand;
  int bincandC;
  /***** -onoff: A list of white-space separated pairs of numbers from 0.0 to 1.0 that designate barycentric times in our data set when we will actually keep the data. (i.e. '-onoff 0.1 0.4 0.7 0.9' means that we will fold the data set during the barycentric times 0.1-0.4 and 0.7-0.9 of the total time length of the data set) */
  char onoffP;
  char* onoff;
  int onoffC;
  /***** -bin: Fold a binary pulsar.  Must include all of the following parameters */
  char binaryP;
  /***** -pb: The orbital period (s) */
  char pbP;
  double pb;
  int pbC;
  /***** -x: The projected orbital semi-major axis (lt-sec) */
  char asinicP;
  double asinic;
  int asinicC;
  /***** -e: The orbital eccentricity */
  char eP;
  double e;
  int eC;
  /***** -To: The time of periastron passage (MJD) */
  char ToP;
  double To;
  int ToC;
  /***** -w: Longitude of periastron (deg) */
  char wP;
  double w;
  int wC;
  /***** -wdot: Rate of advance of periastron (deg/yr) */
  char wdotP;
  double wdot;
  int wdotC;
  /***** uninterpreted command line parameters */
  int argc;
  /*@null*/char **argv;
  /***** the whole command line concatenated */
  char *full_cmd_line;
} Cmdline;


extern char *Program;
extern void usage(void);
extern /*@shared*/Cmdline *parseCmdline(int argc, char **argv);

extern void showOptionValues(void);

#endif

