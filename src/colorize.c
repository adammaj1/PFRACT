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
gcc colorize.c -lm -Wall -Wextra -o colorize

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
#define FLOOR(xx) (float)floor((double)(xx))
#else
#define FLOAT double
#define EXPF  exp
#define POWF  pow
#define SINF  sin
#define COSF  cos
#define FLOOR(xx) floor(xx)
#endif

#define UINT16     unsigned short
#define UINT32     unsigned int

#define MALLOC(nn,tt) (tt *)malloc((nn)*sizeof(tt))
#define FREE(dd) free(dd)

#define DEG2UNIT(xx) ((xx)*(1.0/360.0))
#define UNIT2DEG(xx) ((xx)*360.0)

UINT16 fread_uint16(FILE *fp) {
  UINT16 val;

  val  = fgetc(fp)<<8;
  val |= fgetc(fp);
  return val;
}

UINT32 fread_uint32(FILE *fp) {
  UINT32 val;

  val  = fgetc(fp)<<24;
  val |= fgetc(fp)<<16;
  val |= fgetc(fp)<<8;
  val |= fgetc(fp);
  return val;
}

void hsv2rgb(FLOAT h, FLOAT s, FLOAT v, FLOAT *r, FLOAT *g, FLOAT *b) {
  int   k;
  FLOAT f, t, n, q;

  if (h>1.0 || h<0.0) h = h - FLOOR(h);
  h *= 6.0; h -= 0.00001;
  if (h>0.0) k = (int)FLOOR(h); else k = 0;
  f = h - (FLOAT)k;
  t = 1 - s;
  n = 1 - s*f;
  q = 1 - s*(1-f);
  switch(k) {
  case 0: *r = 1; *g = q; *b = t; break;
  case 1: *r = n; *g = 1; *b = t; break;
  case 2: *r = t; *g = 1; *b = q; break;
  case 3: *r = t; *g = n; *b = 1; break;
  case 4: *r = q; *g = t; *b = 1; break;
  case 5: *r = 1; *g = t; *b = n; break;
  }
  n = (*r>*g?*r:*g); n = (n>*b?n:*b);
  f = v/n; *r *= f; *g *= f; *b *= f;
}


static UINT16 map16_maxiter = 1, map16_fo = 0;
static UINT32 map32_maxiter = 1, map32_fo = 0;
static FLOAT  map_cscale = 1.0, map_mulf = 1.0, map_muls = 1.0/20.0;
static FLOAT  map_starta=0.0, map_spawn = 1.0;

void map16_init(UINT16 maxiter, FLOAT cscale, int fo, int fd, FLOAT starta,
		FLOAT spawn) {
  map16_fo      = (UINT16)fo;
  map16_maxiter = maxiter + map16_fo;
  map_cscale    = cscale;
  map_mulf      = 1.0/(FLOAT)(maxiter - map16_fo);
  map_muls      = 1.0/(FLOAT)fd;
  map_starta    = starta - FLOOR(starta);
  map_spawn     = spawn;
}

void map32_init(UINT32 maxiter, FLOAT cscale, int fo, int fd, FLOAT starta,
		FLOAT spawn) {
  map32_fo      = (UINT32)fo;
  map32_maxiter = maxiter + map32_fo;
  map_cscale    = cscale;
  map_mulf      = 1.0/(FLOAT)(maxiter - map32_fo);
  map_muls      = 1.0/(FLOAT)fd;
  map_starta    = starta - FLOOR(starta);
  map_spawn     = spawn;
}

void map16(UINT16 val, FILE *fop) {
  FLOAT r, g, b, f, s;

  if (val<=map16_fo) { f = map_starta; s = 0.0; }
  else {
    f = POWF((FLOAT)(map16_maxiter-val)*map_mulf, map_cscale)*map_spawn
      + map_starta;
    s = 1.0-EXPF(-(FLOAT)(val-map16_fo)*map_muls);
  }
  hsv2rgb(f, s, 1, &r, &g, &b);
  fputc((unsigned char)(r*255.0), fop);
  fputc((unsigned char)(g*255.0), fop);
  fputc((unsigned char)(b*255.0), fop);
}

void map32(UINT32 val, FILE *fop) {
  FLOAT r, g, b, f, s;

  if (val<=map16_fo) { f = map_starta; s = 0.0; }
  else {
    f = POWF((FLOAT)(map32_maxiter-val)*map_mulf, map_cscale)*map_spawn
      + map_starta;
    s = 1.0-EXPF(-(FLOAT)(val-map32_fo)*map_muls);
  }
  hsv2rgb(f, s, 1, &r, &g, &b);
  fputc((unsigned char)(r*255.0), fop);
  fputc((unsigned char)(g*255.0), fop);
  fputc((unsigned char)(b*255.0), fop);
}

int main(int argc, char *argv[]) {
  FILE          *fip = stdin, *fop = stdout;
  FLOAT          cscale = 1.0, starta = 0.0, spawn = 1.0;
  unsigned char  inset[3] = { 0, 0, 0 };
  int            errors=0, optind=1, args=0,  i; // maxiter=256, do_julia = 0,
  int            fadeout=0, fadedepth=20, show_info = 0;
  unsigned char  magic[MAGIC_SIZE];
  UINT16         width, height, bytes, val16, max16;
  UINT32         val32, max32;
  
  while(!errors && optind<argc) {
    if (argv[optind][0]=='-' && argv[optind][1]!='-') {
      if (strcmp(argv[optind], "-scale")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -scale.\n");
	  errors++;
	} else {
	  cscale = atof(argv[++optind]);
	}
      } else if (strcmp(argv[optind], "-start")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -start.\n");
	  errors++;
	} else {
	  starta = DEG2UNIT(atof(argv[++optind]));
	}
      } else if (strcmp(argv[optind], "-spawn")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -spawn.\n");
	  errors++;
	} else {
	  spawn = DEG2UNIT(atof(argv[++optind]));
	}
      } else if (strcmp(argv[optind], "-inset")==0) {
	if (argc-optind<4) {
	  fprintf(stderr, "Error: Not enough values for -inset.\n");
	  errors++;
	} else {
	  inset[0] = atoi(argv[++optind]);
	  inset[1] = atoi(argv[++optind]);
	  inset[2] = atoi(argv[++optind]);
	}
      } else if (strcmp(argv[optind], "-fadeout")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -fadeout.\n");
	  errors++;
	} else {
	  fadeout = atoi(argv[++optind]);
	}
      } else if (strcmp(argv[optind], "-fadedepth")==0) {
	if (argc-optind<2) {
	  fprintf(stderr, "Error: Not enough values for -fadedepth.\n");
	  errors++;
	} else {
	  fadedepth = atoi(argv[++optind]);
	}
      } else if (strcmp(argv[optind], "-info")==0) {
	show_info = 1;
      } else if (strcmp(argv[optind], "-h")==0) {
	errors++;
      } else if (strcmp(argv[optind], "-help")==0) {
	errors++;
      } else {
	fprintf(stderr, "Error: Unknown option '%s'\n", argv[optind]);
	errors++;
      }
    } else {
      switch(args++) {
      case 0:
	if (argv[optind][0]!='-') 
	  if ((fip=fopen(argv[optind], "rb"))==NULL) {
	    fprintf(stderr, "Cannot open file %s for reading.\n",
		    argv[optind]);
	    errors++;
	  }
	break;
      case 1:
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
    fprintf(stderr,"|                 Iteration file colorizer                 |\n");
    fprintf(stderr,"|    (C) Mika Seppa 1996 (http://neuro.hut.fi/~mseppa/)    |\n");
    fprintf(stderr,"------------------------------------------------------------\n");
    fprintf(stderr,"Usage: %s [options] [infile.raw] [outfile.ppm]\n",argv[0]);
    fprintf(stderr," -fadeout ...     : Fades out to white (%d)\n", fadeout);
    fprintf(stderr," -fadedepth ...   : Depth of fading (%d)\n", fadedepth);
    fprintf(stderr," -help            : This help\n");
    fprintf(stderr," -info            : Show information about file\n");
    fprintf(stderr," -inset .. .. ..  : Color in the set (%d,%d,%d)\n", 
	    inset[0], inset[1], inset[2]);
    fprintf(stderr," -scale ...       : Color scale (%g)\n", cscale);
    fprintf(stderr," -spawn ...       : Color angle to spawn (%.0f)\n",
	    UNIT2DEG(spawn));
    fprintf(stderr," -start ...       : Starting color angle (%.0f)\n",
	    UNIT2DEG(starta));
    fprintf(stderr," --               : Use stdin instead of file\n");
    exit(1);
  }

  fread(magic, MAGIC_SIZE, 1, fip);
  for(i=0;i<MAGIC_SIZE;i++) if (magic[i]!=MAGIC_NUMBER[i]) {
    fprintf(stderr, "%s: Error, file is not a iteration file.\n", argv[0]);
    exit(1);
  }

  width  = fread_uint16(fip);
  height = fread_uint16(fip);
  bytes  = fread_uint16(fip);

  if (bytes!=2 && bytes!=4) {
    fprintf(stderr, "%s: Error, wrong number per bytes (%d).\n", argv[0], bytes);
    exit(2);
  }

  fprintf(fop, "P6\n%d %d\n255\n", width, height);

  switch(bytes) {
  case 2:
    max16 = fread_uint16(fip);
    if (show_info) fprintf(stderr, "2-byte file: size %dx%d, maxiter %d\n",
			   width, height, max16);
    map16_init(max16, cscale, fadeout, fadedepth, starta, spawn);
    for(i=width*height;i-->0;) {
      val16 = fread_uint16(fip);
      if (val16==max16) {
	fputc(inset[0], fop);
	fputc(inset[1], fop);
	fputc(inset[2], fop);
      } else map16(val16, fop);
    }
    break;

  case 4:
    max32 = fread_uint32(fip);
    if (show_info) fprintf(stderr, "4-byte file: size %dx%d, maxiter %d\n",
			   width, height, max32);
    map32_init(max32, cscale, fadeout, fadedepth, starta, spawn);
    for(i=width*height;i-->0;) {
      val32 = fread_uint32(fip);
      if (val32==max32) {
	fputc(inset[0], fop);
	fputc(inset[1], fop);
	fputc(inset[2], fop);
      } else map32(val32, fop);
    }
    break;

  }

  exit(0);
  return 0;
}
