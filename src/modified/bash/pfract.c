/*
neuro.hut.fi/~mseppa/images/Fract.html 

Fractals in polar coordinates by Mika Seppä
"I have written two programs to calculate Mandelbrot and Julia-sets in polar coordinates. 
The first program called pfract calculates the actual iterations into a special file (see specs), 
and the other program is then used for colorizing the results to get a PPM-image."

This program does the actual calculation of the fractals. 


It's output is a raw-file where the iterations are stored. 


The coordinate system is such that the angular component goes to the y-axis, and the radius
goes to the x-axis of the resulting image. In addition, the x-axis (radius) is normalized with exp-function so that angles are preserved in this mapping. Here is the list of available options:


> pfract -h
------------------------------------------------------------
| Mandelbrot- and Julia-set fractals in polar coordinates. |
| (C) Mika Seppa 1996 (email: my.email.address) |
------------------------------------------------------------
Usage: pfract [options] [outfile.raw]
 -cx ... : Start-point x-coordinate (0)
 -cy ... : Start-point y-coordinate (0)
 -help : This help
 -iterations ... : Maximum iterations (255)
 -julia : Calculate Julia-set
 -rad ... : Minimum radius (-5)
 -size ... ... : Size of the image (256x256)
 -x ... : Origin x-coordinate (0)
 -y ... : Origin y-coordinate (0)
 
 
 where :
-cx,-cy: The seed-point from Mandelbrot-set for the Julia-set.
-help,-h: The help shown above.
-iterations: The maximum iterations value.
-julia: Calculate Julia-set instead of Mandelbrot-set.
-rad: Minimum radius. This is the value before the exponential mapping, so negative values just mean radiuses below 1. The maximum radius is calculated from this value and the size of the target image.
-size: Size of the target image. Use very wide images for background purposes.
-x,-y: The origin of the polar-coordinates. If you find a nice region from the Mandelbrot-set, use these values for -cx,-cy -options and try to calculate a Julia-set.

===========================================================

COLORIZE
This program reads the raw-output from the pfract program and colorizes it into a PPM-file. With the help of options shown below, one has a simple control over the colorizing process.
> colorize -h
------------------------------------------------------------
| Iteration file colorizer |
| (C) Mika Seppa 1996 (email: my.email.address) |
------------------------------------------------------------
Usage: colorize [options] [infile.raw] [outfile.ppm]
 -fadeout  	: Fades out to white (0)
 -fadedepth 	 : Depth of fading (20)
 -help 		: This help
 -info 		: Show information about file
 -inset 	: Color in the set (0,0,0)
 -scale  	: Color scale (1)
 -spawn  	: Color angle to spawn (360)
 -start  	: Starting color angle (0)
 -- 		: Use stdin instead of file
 
 
 
-fadeout: Iteration count below which the color goes to white.
-fadedepth: How many steps (iterations) to use for fading (saturating) the white to a color.
-help,-h: The help shown above.
-info: Some information about the raw-file.
-inset: What color to use for pixels inside the Mandelbrot or Julia-set. Default value is black.
-scale: Determines how fast the color changes in the low and high-iteration areas of the image. A kind of gamma correction for iteration values.
-spawn: The color-angle to spawn in degrees.
-start: The offset for color-angle in degrees.
--: Use -- in place of inputfile if you want to specify an outfile, but the input comes through a pipe.


Just try different options to get a pleasing image. It seems to be good to increase the -scale option if the maximum iterations in the raw-file is high.


====================================================

Here you can find some example runs of the pfract and colorize programs.
pfract -julia -cx -1.115313 -cy -0.304872 -x -0.147394 -y -0.234163 -size 1600 160 -iterations 164 -rad -8 example1.raw
colorize example1.raw example1.ppm -fadedepth 20 -scale 1.4
pfract -julia -cx 0.362927 -cy -0.077798 -x -0.274848 -y -0.908705 -size 1600 192 -iterations 164 -rad -5.6 example2.raw
colorize example2.raw example2.ppm -start 270 -spawn -90 -fadedepth 60
pfract -julia -cx 0.362927 -cy -0.077798 -x -0.282406 -y -0.912345 -size 1600 160 -iterations 256 -rad -9 example3.raw
colorize example3.raw example3.ppm -start 200 -spawn -300 -scale 1.4 -fadedepth 50
pfract -julia -cx -1.769705 -cy -0.003474 -x -0.015064 -y -0.036825 -size 1600 160 -iterations 256 -rad -8.3 example4.raw
colorize example4.raw example4.ppm -spawn -360
pfract -x 0.360211 -y -0.684246 -size 1600 160 -iterations 256 -rad -10 example5.raw
colorize example5.raw example5.ppm -spawn -400 -scale 1.6 -fadedepth 25
pfract -julia -cx -0.771893 -cy -0.111656 -x -0.101176 -y -0.192846 -size 1600 160 -iterations 512 -rad -7.2 example6.raw
colorize example6.raw example6.ppm -spawn -360 -start 45 -fadedepth 100 -scale 1.5

----------------------------------------------

gcc pfract.c -lm -Wall -Wextra -o pfract

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> // strcmp



#define MAGIC_NUMBER "ITERFILE"
#define MAGIC_SIZE   8

#ifdef NODOUBLE
#define FLOAT float
#define EXPF  expf
#define POWF  powf
#define SINF  sinf
#define COSF  cosf
#else
#define FLOAT double
#define EXPF  exp
#define POWF  pow
#define SINF  sin
#define COSF  cos
#endif

#define UINT16     unsigned short
#define UINT32     unsigned int

#ifdef BIG_ITERATIONS
#define IVAL        UINT32
#define FWRITE_IVAL fwrite_uint32
#else
#define IVAL        UINT16
#define FWRITE_IVAL fwrite_uint16
#endif

#define MALLOC(nn,tt) (tt *)malloc((nn)*sizeof(tt))
#define FREE(dd) free(dd)

#define MAXSIZE 20048

void fwrite_uint16(UINT16 val, FILE *fp) {
  fputc((val>>8)&0xff, fp);
  fputc(val&0xff, fp);
}

void fwrite_uint32(UINT32 val, FILE *fp) {
  fputc((val>>24)&0xff, fp);
  fputc((val>>16)&0xff, fp);
  fputc((val>>8)&0xff, fp);
  fputc(val&0xff, fp);
}

IVAL iterate_julia(IVAL maxiter, FLOAT pr, FLOAT pi, FLOAT jr, FLOAT ji) {
  FLOAT  f[2], f2[2];
  IVAL i;

  for(i=0,f[0]=pr,f[1]=pi;i<maxiter;i++) {
    f2[0] = f[0]*f[0]; f2[1] = f[1]*f[1];
    if (f2[0]+f2[1]>25.0) break;
    
    /* Iterate */
    f[1] = 2.0*f[0]*f[1] + ji;
    f[0] = f2[0] - f2[1] + jr;
  }
  return i;
}

IVAL iterate_mandel(IVAL maxiter, FLOAT pr, FLOAT pi) {
  FLOAT  f[2], f2[2];
  IVAL i;

  for(i=0,f[0]=pr,f[1]=pi;i<maxiter;i++) {
    f2[0] = f[0]*f[0]; f2[1] = f[1]*f[1];
    if (f2[0]+f2[1]>25.0) break;
    
    /* Iterate */
    f[1] = 2.0*f[0]*f[1] + pi;
    f[0] = f2[0] - f2[1] + pr;
  }
  return i;
}

void julia(IVAL *data, FLOAT *origin, FLOAT *radius, int *size,
	   FLOAT *juliao, IVAL maxiter) {	// , FLOAT cscale
  FLOAT f, sf, cf, mulx, muly;
  IVAL  i;
  int   x, y;

  mulx = (radius[1]-radius[0])/(FLOAT)size[0];
  muly = 1.0/(FLOAT)size[1]*2.0*3.14159265;
  for(y=0;y<size[1];y++) {
    f  = (FLOAT)y*muly;
    sf = SINF(f);
    cf = COSF(f);
    for(x=0;x<size[0];x++) {
      f = EXPF((FLOAT)x*mulx+radius[0]);
      i = iterate_julia(maxiter, f*cf+origin[0], f*sf+origin[1],
			juliao[0], juliao[1]);
      if (i==0) for(;x<size[0];x++) *data++ = 0;
      else *data++ = i;
    }
  }
}

void mandel(IVAL *data, FLOAT *origin, FLOAT *radius, int *size,
	    IVAL maxiter) { 	// , FLOAT cscale
  FLOAT f, sf, cf, mulx, muly;
  IVAL  i;
  int   x, y;

  mulx = (radius[1]-radius[0])/(FLOAT)size[0];
  muly = 1.0/(FLOAT)size[1]*2.0*3.14159265;
  for(y=0;y<size[1];y++) {
    f = (FLOAT)y*muly;
    sf = SINF(f);
    cf = COSF(f);
    for(x=0;x<size[0];x++) {
      f = EXPF((FLOAT)x*mulx+radius[0]);
      i = iterate_mandel(maxiter, f*cf+origin[0], f*sf+origin[1]);
      if (i==0) for(;x<size[0];x++) *data++ = 0;
      else *data++ = i;
    }
  }
}

int  main(int argc, char *argv[]) {
  FLOAT   origin[2] = {  0.0, 0.0 };
  FLOAT   juliao[2] = {  0.0, 0.0 };
  FLOAT   radius[2] = { -5.0, 2.0 };
  int     size[2]   = {  256, 256 };
  FILE   *fop = stdout;
  IVAL    maxiter = 255;
  int     errors=0, optind=1, args=0, do_julia = 0, i;
 // FLOAT   cscale = 1.0;
  IVAL   *data = NULL, *p;

  while(!errors && optind<argc) {
    if (argv[optind][0]=='-' && argv[optind][1]!='-') {
      if (strcmp(argv[optind], "-x")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -x.\n");
	  errors++;
	} else {
	  origin[0] = atof(argv[++optind]);
	}
      }	else if (strcmp(argv[optind], "-y")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -y.\n");
	  errors++;
	} else {
	  origin[1] = atof(argv[++optind]);
	}
      }	else if (strcmp(argv[optind], "-cx")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -cx.\n");
	  errors++;
	} else {
	  juliao[0] = atof(argv[++optind]);
	}
      }	else if (strcmp(argv[optind], "-cy")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -cy.\n");
	  errors++;
	} else {
	  juliao[1] = atof(argv[++optind]);
	}
      } else if (strcmp(argv[optind], "-help")==0) {
	errors++;
      } else if (strcmp(argv[optind], "-h")==0) {
	errors++;
      } else if (strcmp(argv[optind], "-julia")==0) {
	do_julia = 1;
      } else if (strcmp(argv[optind], "-iterations")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -iterations.\n");
	  errors++;
	} else {
	  maxiter = (IVAL)atoi(argv[++optind]);
	}
      } else if (strcmp(argv[optind], "-rad")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -rad.\n");
	  errors++;
	} else {
	  radius[0] = atof(argv[++optind]);
	}
      } else if (strcmp(argv[optind], "-size")==0) {
	if (argc-optind<3) {
	  fprintf(stderr, "Error: Not enough values for -size.\n");
	  errors++;
	} else {
	  size[0] = atoi(argv[++optind]);
	  size[1] = atoi(argv[++optind]);
	}
      } else {
	fprintf(stderr, "Error: Unknown option '%s'\n", argv[optind]);
	errors++;
      }
    } else {
      switch(args++) {
      case 0:
	if (argv[optind][0]!='-') 
	  if ((fop=fopen(argv[optind], "wb"))==NULL) {
	    fprintf(stderr, "Cannot open file %s for writing.\n",
		    argv[optind]);
	    errors++;
	  }
	break;
      default:
	fprintf(stderr, "Strange argument '%s'.\n", argv[optind]);
      }
    }
    optind++;
  }

  if (errors) {
    fprintf(stderr,"------------------------------------------------------------\n");
    fprintf(stderr,"| Mandelbrot- and Julia-set fractals in polar coordinates. |\n");
    fprintf(stderr,"|    (C) Mika Seppa 1996 (http://neuro.hut.fi/~mseppa/)    |\n");
    fprintf(stderr,"------------------------------------------------------------\n");
    fprintf(stderr,"Usage: %s [options] [outfile.raw]\n",argv[0]);
    fprintf(stderr," -cx ...             : Start-point x-coordinate (%g)\n",
	    juliao[0]);
    fprintf(stderr," -cy ...             : Start-point y-coordinate (%g)\n",
	    juliao[1]);
    fprintf(stderr," -help               : This help\n");
    fprintf(stderr," -iterations ...     : Maximum iterations (%d)\n",
	    maxiter);
    fprintf(stderr," -julia              : Calculate Julia-set\n");
    fprintf(stderr," -rad ...            : Minimum radius (%g)\n",
	    radius[0]);
    fprintf(stderr," -size ... ...       : Size of the image (%dx%d)\n",
	    size[0], size[1]);
    fprintf(stderr," -x ...              : Origin x-coordinate (%g)\n",
	    origin[0]);
    fprintf(stderr," -y ...              : Origin y-coordinate (%g)\n",
	    origin[1]);
    exit(1);
  }

  if (size[0]<1 || size[0]>MAXSIZE || size[1]<1 || size[1]>MAXSIZE) {
    fprintf(stderr, "%s: Error, illegal size %dx%d\n", argv[0], size[0],
	    size[1]);
    exit(1);
  }

  data = MALLOC(size[0]*size[1], IVAL);
  
  radius[1] = radius[0] + (FLOAT)size[0]/(FLOAT)size[1] * 2.0 * 3.14159265;

  if (do_julia) julia(data, origin, radius, size, juliao, maxiter); // , cscale
  else mandel(data, origin, radius, size, maxiter); // , cscale

  fwrite(MAGIC_NUMBER, MAGIC_SIZE, 1, fop);
  fwrite_uint16((UINT16)size[0], fop);
  fwrite_uint16((UINT16)size[1], fop);
  fwrite_uint16((UINT16)sizeof(IVAL), fop);
  FWRITE_IVAL(maxiter, fop);

  i = size[0]*size[1];
  for(p=data;i-->0;) FWRITE_IVAL(*p++, fop);
  FREE(data);

  exit(0);
  
  return 0;
  
}
