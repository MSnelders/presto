#include "cpgplot.h"
#include "ransomfft.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

void find_min_max_arr(int npts, float *arr, float *min, float *max);
void dfind_min_max_arr(int npts, double *arr, double *min, double *max);
void xyline(int npts, float *x, float *y, const char *xlab, \
	    const char *ylab, int id);
void dxyline(int npts, double *x, double *y, const char *xlab, \
	     const char *ylab, int id);
void xyline2lab(int npts, float *x, float *y, float *y2, const char *xlab, \
		const char *ylab, const char *ylab2, int id);
void powerplot(int npts, float *freqs, float *amp, float norm, int id);
void cpgstart_ps(const char *filenm, const char *orientation);
void cpgstart_x(const char *orientation);
double plot_power(double rl, double im);
void multi_prof_plot(int proflen, int numprofs, double *profiles, \
		     double *sumprof, const char *xlbl, \
		     double loly, double ldy, const char *lylbl, \
		     double lory, double rdy, const char *rylbl);

