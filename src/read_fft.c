#include "chkio.h"
#include "ransomfft.h"

fcomplex *read_fcomplex_file(FILE *file, int firstpt, int numpts)
/* Return an fcomplex vector with complex data taken from a file. */
/* Argumants:                                                     */
/*   'file' is a pointer to the file you want to access.          */
/*   'firstpt' is the number of the first point to get. (0 = 1st  */
/*       point in the file).  If < 0, the resulting array will    */
/*       be zero padded.                                          */
/*   'numpts' is the number of points to get from the file.       */
/*       If the number of bins to read takes us past the end of   */
/*       file, the returned vector will be zero padded.           */
{
  int ii, startpt = 0, lesspad = 0;
  fcomplex *result, *fcptr=NULL, zeros = {0.0,0.0};
  
  if (numpts < 0){
    printf("\n\n numpts = %d (out-of-bounds) in read_fcomplex_file().", \
	   numpts);
    printf("  Exiting.\n\n");
    exit(1);
  }
  
  /* Allocate the result array */
  
  result = gen_cvect(numpts);
  for (ii = 0; ii < numpts; ii++) result[ii] = zeros;
  
  /* Zero pad if we try to read before the beginning of the file */
  
  if (firstpt < 0){
    fcptr = result + abs(firstpt);
    lesspad = abs(firstpt);
    startpt = 0;
  } else {
    fcptr = result;
    startpt = firstpt;
    lesspad = 0;
  }
  
  /* Position and read the data */
  
  chkfileseek(file, startpt, sizeof(fcomplex), SEEK_SET);
  chkfread(fcptr, sizeof(fcomplex), numpts-lesspad, file);
  return result;
}
