#include <limits.h>
#include "presto.h"
#include "multibeam.h"
#include "readfile_cmd.h"

/* #define DEBUG */

#define PAGELEN 32   /* Set the page length to 32 lines */
#define NUMTYPES 9

int FLOAT_print(long count, char *obj_ptr);
int DOUBLE_print(long count, char *obj_ptr);
int FCPLEX_print(long count, char *obj_ptr);
int DCPLEX_print(long count, char *obj_ptr);
int INT_print(long count, char *obj_ptr);
int LONG_print(long count, char *obj_ptr);
int RZWCAND_print(long count, char *obj_ptr);
int BINCAND_print(long count, char *obj_ptr);
int PKMBHDR_print(long count, char *obj_ptr);

typedef enum{
  FLOAT, DOUBLE, FCPLEX, DCPLEX, INT, LONG, RZWCAND, BINCAND, PKMBHDR
} rawtypes;

typedef struct fcplex{
  float r;
  float i;
} fcplex;

typedef struct dcplex{
  double r;
  double i;
} dcplex;

int type_sizes[NUMTYPES] = {
  sizeof(float), \
  sizeof(double), \
  sizeof(fcplex), \
  sizeof(dcplex), \
  sizeof(int), \
  sizeof(long), \
  sizeof(fourierprops), \
  sizeof(binaryprops), \
  49792  /* This is the length of a Parkes Multibeam record */
};

int objs_at_a_time[NUMTYPES] = {
  PAGELEN, PAGELEN, PAGELEN, PAGELEN, PAGELEN, PAGELEN, 1, 1, 1
};

/* You don't see this every day -- An array of pointers to functions: */

int (*print_funct_ptrs[NUMTYPES])() = {
  FLOAT_print, \
  DOUBLE_print, \
  FCPLEX_print, \
  DCPLEX_print, \
  INT_print, \
  LONG_print, \
  RZWCAND_print, \
  BINCAND_print, \
  PKMBHDR_print
};

/* A few global variables */

long N;
double dt, nph;

int main(int argc, char **argv)
{
  int index = -1, need_type = 0, objs_read, objs_to_read;
  long i, j, ct;
  char *ctmp, *data, *short_filenm, *extension, key = '\n';
  FILE *infile;
  Cmdline *cmd;
  infodata inf;

  /* Call usage() if we have no command line arguments */

  if (argc == 1) {
    Program = argv[0];
    usage();
    exit(0);
  }

  /* Parse the command line using the excellent program Clig */

  cmd = parseCmdline(argc, argv);

#ifdef DEBUG
  showOptionValues();
#endif

  fprintf(stderr, "\n\n  PRESTO Binary File Reader\n");
  fprintf(stderr, "     by Scott M. Ransom\n");
  fprintf(stderr, "       20 March 1999\n\n");

  /* Set our index value */

  if (cmd->fltP || cmd->sfltP) index = FLOAT;
  else if (cmd->dblP || cmd->sdblP) index = DOUBLE;
  else if (cmd->fcxP || cmd->sfcxP) index = FCPLEX;
  else if (cmd->dcxP || cmd->sdcxP) index = DCPLEX;
  else if (cmd->igrP || cmd->sigrP) index = INT;
  else if (cmd->lngP || cmd->slngP) index = LONG;
  else if (cmd->rzwP || cmd->srzwP) index = RZWCAND;
  else if (cmd->binP || cmd->sbinP) index = BINCAND;
  else if (cmd->pksP || cmd->spksP) index = PKMBHDR;

  /* Try to determine the data type from the file name */

  if (index == -1 || index == RZWCAND){
    short_filenm = (char *)malloc(strlen(cmd->argv[0]) + 1);
    extension = strrchr(cmd->argv[0], '.');
    if (extension == NULL ||
	extension <= cmd->argv[0] + 3){
      need_type = 1;
    } else {
      if (strlen(extension) < 4){
	need_type = 1;
      } else {
	ctmp = (char *)malloc(strlen(extension) + 1);
	strncpy(ctmp, extension, strlen(extension) + 1);
	if (0 == strcmp(ctmp, ".dat")){
	  index = FLOAT;
	  fprintf(stderr, \
		  "Assuming the data is floating point.\n\n");
	} else if (0 == strcmp(ctmp, ".fft")){
	  index = FCPLEX;
	  fprintf(stderr, \
		  "Assuming the data is single precision complex.\n\n");
	} else if (0 == strcmp(ctmp, ".can")){
	  /* A binary or RZW search file? */
	  strncpy(ctmp, extension - 4, 4);
	  if (0 == strcmp(ctmp, "_bin")){
	    index = BINCAND;
	    fprintf(stderr, \
		    "Assuming the file contains binary candidates.\n\n");
	  } else if (NULL != (extension = strstr(cmd->argv[0], "_rzw"))){
	    index = RZWCAND;
	    ct = (long) (extension - cmd->argv[0]);
	    fprintf(stderr, \
		    "Assuming the file contains 'RZW' candidates.\n");
	    free(short_filenm);
	    short_filenm = (char *)malloc(ct + 1);
	    strncpy(short_filenm, cmd->argv[0], ct);	    
	    fprintf(stderr, \
		    "\nAttempting to read '%s.inf'.  ", short_filenm);
	    readinf(&inf, short_filenm);
	    fprintf(stderr, "Successful.\n");
	    N = (long)(inf.N + DBLCORRECT);
	    dt = inf.dt;
	    if (cmd->nphP)nph = cmd->nph;
	    else nph = 1.0;
	    fprintf(stderr, \
		    "\nUsing N = %ld, dt = %g, and DC Power = %f\n\n", \
		    N, dt, nph);
	  }
	  else need_type = 1;
	} else need_type = 1;
      }
    }

    /* If no file extension or if we don't understand the extension, exit */

    if (need_type){
      fprintf(stderr, \
	      "You must specify a data type for this file.\n\n");
      free(short_filenm);
      exit(-1);
    }
    free(short_filenm);
  }

  if (cmd->index[1] == -1 || cmd->index[1] == 0) cmd->index[1] = INT_MAX;
  if (cmd->index[1] < cmd->index[0]){
    fprintf(stderr, "\nThe high index must be >= the low index.");
    fprintf(stderr, "  Exiting.\n\n");
    exit(-1);
  }

  /* Open the file */

  infile = chkfopen(cmd->argv[0], "rb");

  /* Skip to the correct first object */

  if (cmd->index[0] > 0){
    chkfileseek(infile, (long) (cmd->index[0]), type_sizes[index], \
		SEEK_SET);
  }

  /* Read the file */

  objs_to_read = objs_at_a_time[index];
  data = (char *)malloc(type_sizes[index] * objs_at_a_time[index]);

  i = cmd->index[0];
  do {
    if (objs_to_read > cmd->index[1] - i)
      objs_to_read = cmd->index[1] - i;
    objs_read = chkfread(data, type_sizes[index], objs_to_read, infile);
    for(j = 0; j < objs_read; j++)
      print_funct_ptrs[index](i + j, data + j * type_sizes[index]);
    i += objs_read;
    if (cmd->moreP){
      fprintf(stderr, "\nPress ENTER for next page, or any other key and ");
      fprintf(stderr, "then ENTER to exit.\n\n");
      key = getchar();
    }
  } while (!feof(infile) && i < cmd->index[1] && key == '\n');

  if (feof(infile)){
    fprintf(stderr, "\nEnd of file.\n\n");
  }

  free(data);
  fclose(infile);
  exit(0);
 }


int FLOAT_print(long count, char *obj_ptr)
{
  float *object;

  object = (float *) obj_ptr;
  printf("%9ld:  %12.6g\n", count, *object);
  return 0;
}

int DOUBLE_print(long count, char *obj_ptr)
{
  double *object;

  object = (double *) obj_ptr;
  printf("%9ld:  %20.14g\n", count, *object);
  return 0;
}

int FCPLEX_print(long count, char *obj_ptr)
{
  fcplex *object;

  object = (fcplex *) obj_ptr;
  printf("%9ld:  %12.6g + %12.6g i\n", count, object->r, object->i);
  return 0;
}

int DCPLEX_print(long count, char *obj_ptr)
{
  dcplex *object;

  object = (dcplex *) obj_ptr;
  printf("%9ld:  %20.14g + %20.14g i\n", count, object->r, object->i);
  return 0;
}

int INT_print(long count, char *obj_ptr)
{
  int *object;

  object = (int *) obj_ptr;
  printf("%9ld:  %10d\n", count, *object);
  return 0;
}

int LONG_print(long count, char *obj_ptr)
{
  long *object;

  object = (long *) obj_ptr;
  printf("%9ld:  %12ld\n", count, *object);
  return 0;
}

int RZWCAND_print(long count, char *obj_ptr)
{
  fourierprops *object;

  object = (fourierprops *) obj_ptr;
  printf("\n%ld:\n", count + 1);
  print_candidate(object, dt, N, nph, 2);
  return 0;
}

int BINCAND_print(long count, char *obj_ptr)
{
  binaryprops *object;

  object = (binaryprops *) obj_ptr;
  printf("\n%ld:\n", count + 1);
  print_bin_candidate(object, 2);
  return 0;
}

int PKMBHDR_print(long count, char *obj_ptr)
{
  multibeam_tapehdr *object;

  object = (multibeam_tapehdr *) obj_ptr;
  printf("\n%ld:", count + 1);
  print_multibeam_hdr(object);
  return 0;
}

