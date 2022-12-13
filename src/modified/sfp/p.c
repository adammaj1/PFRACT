/*

gcc p.c -lm -Wall -Wextra 



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


#define IVAL        UINT16
#define FWRITE_IVAL fwrite_uint16


#define MALLOC(nn,tt) (tt *)malloc((nn)*sizeof(tt))
#define FREE(dd) free(dd)

#define MAXSIZE 20048




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




// c = pr + pi*I
IVAL iterate_mandel(IVAL maxiter, FLOAT pr, FLOAT pi) {
  	FLOAT  f[2], f2[2];
  	IVAL i;

  	for(i=0,f[0]=pr,f[1]=pi;i<maxiter;i++) {
    
  		f2[0] = f[0]*f[0]; // Zx2=Zx*Zx;
    		f2[1] = f[1]*f[1]; // Zy2=Zy*Zy;
    		
    		if (f2[0]+f2[1]>25.0) break; // if (Zx2+Zy2) > ER2
    
    		/* Iterate */
    		f[1] = 2.0*f[0]*f[1] + pi; // Zy 
    		f[0] = f2[0] - f2[1] + pr; // Zx 
  	}
  	return i;
}



void mandel(IVAL *data, FLOAT *origin, FLOAT *radius, int *size,
	    IVAL maxiter) { 	// , FLOAT cscale
  FLOAT f, sf, cf, mulx, muly;
  FLOAT cx;
  FLOAT cy;
  IVAL  i;
  int   x, y;
  
	// data, origin, radius, size, maxiter
	//fprintf(stdout, "Calculate Mandelbrot-set.\n");
	//fprintf(stdout, "origin = %f %+f\n", origin[0], origin[1]);
	//fprintf(stdout, "radius = %f %+f\n", radius[0], radius[1]);
	//fprintf(stdout, "size = %d x %d\n", size[0], size[1]);
	//fprintf(stdout, "maxiter = %d\n", maxiter);
	
	
  	mulx = (radius[1]-radius[0])/(FLOAT)size[0];
  	muly = 1.0/(FLOAT)size[1]*2.0*3.14159265;
  	
  	
  	fprintf(stdout, "mulx = %f \n", mulx);
  	fprintf(stdout, "muly = %f \n", muly);
  	
  	fprintf(stderr, "mandel progress info\n"); // progress information
  	for(y=0;y<size[1];y++) {
    		f = (FLOAT)y*muly;
    		sf = SINF(f);
    		cf = COSF(f);
    		fprintf(stderr, " %d / %d\r", y, size[1]); // progress information
    		for(x=0;x<size[0];x++) {
      			f = EXPF((FLOAT)x*mulx+radius[0]);
      			cx = f*cf+origin[0];
      			cy = f*sf+origin[1];
      			//if (y==0 || x==0 || x==size[0]-1 || y == size[1]-1){
      			//	fprintf(stdout, "%f %+f\n", cx, cy);}
      			i = iterate_mandel(maxiter, cx, cy);
      			if (i==0) 
      				for(;x<size[0];x++) *data++ = 0;
      				else *data++ = i;
    		} // for(x=0
  	}
  	
  	fprintf(stderr, " done \n"); // porgress information
} // mandel


/*

pfract [options] [outfile.raw]
 -cx ... : Start-point x-coordinate (0)
 -cy ... : Start-point y-coordinate (0)
 -help : This help
 -iterations ... : Maximum iterations (255)
 -julia : Calculate Julia-set
 -rad ... : Minimum radius (-5)
 -size ... ... : Size of the image (256x256)
 -x ... : Origin x-coordinate (0)
 -y ... : Origin y-coordinate (0)

 pfract -x -1.786440255563638  -y 0 -size 2400 800 -iterations 20000 -rad -20 1.786440255563638.raw
*/
int  main(void ) {
  
  
  
  FLOAT   origin[2] = {  -1.786440255563638, 0.0 }; // c0 
  
  FLOAT   radius[2] = { -20.0, 2.0 };
  int     size[2]   = {  2400, 800 }; // Image width, Image height
  FILE   *raw_file = stdout;
  FILE  *ppm_file = stdout;
  
  IVAL    maxiter = 20000;
  int i;
  IVAL   *data = NULL, *p;

	// colorize
   UINT16         width, height, bytes, val16, max16;
  
   unsigned char  magic[MAGIC_SIZE];
  // cscale, fadeout, fadedepth, starta, spawn
  FLOAT          cscale = 1.5; 
  FLOAT starta = 45.0; // -start  	: Starting color angle (0)
  FLOAT  spawn = - 360.0; // -spawn  	: Color angle to spawn (360)
  int            fadeout=0;
  int  fadedepth=100;
  unsigned char  inset[3] = { 0, 0, 0 };
  // setup 
  data = MALLOC(size[0]*size[1], IVAL);
  
  radius[1] = radius[0] + (FLOAT)size[0]/(FLOAT)size[1] * 2.0 * 3.14159265;


 

  // fill data array
  mandel(data, origin, radius, size, maxiter); // , cscale
  
  
   // save data array to raw file 
  char name [100]; /* name of file */
  snprintf(name, sizeof name, "%.16f", fabs(origin[0])); /*  */
  char *raw_filename =strcat(name,".raw");
  raw_file=fopen(raw_filename, "wb");
  if ( raw_file ==NULL)
  	{fprintf( stderr, "ERROR saving ( cant open) file %s \n", raw_filename); return 1;} 
  	else { fprintf(stderr, "I can open file %s for the first time\n", raw_filename);}
  	
  /*  raw file spec
  	0- 7 : Magic number "ITERFILE"
 	8- 9 : Image width
	10-11 : Image height
	12-13 : Number of bytes per iteration value (NBYTES)
	iF NBYTES=2:
		14-15 : Maximum iterations
		16-   : Data (2 bytes per iteration value)

	IF NBYTES=4:
		14-17 : Maximum iterations
		18-   : Data (4 bytes per iteration value)
*/	
  fwrite(MAGIC_NUMBER, MAGIC_SIZE, 1, raw_file);
  fwrite_uint16((UINT16)size[0], raw_file); //Image width
  fwrite_uint16((UINT16)size[1], raw_file); // Image height
  fwrite_uint16((UINT16)sizeof(IVAL), raw_file); // Number of bytes per iteration value (NBYTES)
  FWRITE_IVAL(maxiter, raw_file); // Maximum iterations
  i = size[0]*size[1];
  for(p=data;i-->0;) 
  	FWRITE_IVAL(*p++, raw_file); // data 
  fclose(raw_file);
  fprintf(stderr, "file %s is closed for the first time\n", raw_filename);
  
  
  
  
  fprintf(stdout,"c0 = %.16f %+.16f it is origin here\n", origin[0], origin[1]);
  fprintf(stdout,"radius minimal = %.16f  \t radius =%+.16f \n", radius[0], radius[1]); 
  fprintf(stdout,"Image width = %d \t Image height = %d\n", size[0], size[1]);
  fprintf(stdout,"Maximum iterations = maxiter = %d \n", maxiter);
  fprintf(stdout,"NBYTES = bytes = %zu so data array has 2 bytes per iteration value\n", sizeof(IVAL));
  
  // ./colorize example8.raw example8.ppm -spawn -400 -scale 1.6 -fadedepth 25
  
  // open raw file for the second time for reading from it
  raw_file = fopen(raw_filename, "rb");
  if (raw_file==NULL) 
  	{ fprintf(stderr, "Cannot open file %s \n", raw_filename); return 2;}
  	else { fprintf(stderr, "I can open file %s for the second time\n", raw_filename);}
  
   fread(magic, MAGIC_SIZE, 1, raw_file);
   for(i=0;i<MAGIC_SIZE;i++) if (magic[i]!=MAGIC_NUMBER[i]) 
  	{
    		fprintf(stderr, "%s: Error, file is not a iteration file.\n", raw_filename);
    		exit(3);
  	}

    width  = fread_uint16(raw_file);
    height = fread_uint16(raw_file);
    bytes  = fread_uint16(raw_file);
  
  
  //open raw file
  
  
   
  if (bytes!=2 && bytes!=4) {
    fprintf(stderr, "%s: Error, wrong number per bytes (%d).\n", raw_filename, bytes);
    exit(2);
  }


  // binary Portable PixMap P6 with .ppm extension
  // open file for writing to it
  snprintf(name, sizeof name, "%.16f", fabs(origin[0])); /*  */
  char *ppm_filename =strcat(name,".ppm");
  ppm_file = fopen(ppm_filename, "wb");
  if (ppm_file==NULL) 
  	{ fprintf(stderr, "Cannot open file %s \n", ppm_filename); return 4;}
  	else { fprintf(stderr, "I can open file %s \n", ppm_filename);}

  fprintf(ppm_file, "P6\n%d %d\n255\n", width, height); // header
  
   max16 = fread_uint16(raw_file);
   fprintf(stdout, "2-byte file: size %dx%d, maxiter %d\n", width, height, max16);
   fprintf(stdout," -scale ...       : Color scale (%g)\n", cscale);
   fprintf(stderr,"------------------------------------------------------------\n");
    fprintf(stderr,"|                 Iteration file colorizer                 |\n");
    fprintf(stderr,"|    (C) Mika Seppa 1996 (http://neuro.hut.fi/~mseppa/)    |\n");
    fprintf(stderr,"------------------------------------------------------------\n");
    fprintf(stderr,"Usage: [options] [infile.raw] [outfile.ppm]\n");
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
   
   
   map16_init(max16, cscale, fadeout, fadedepth, starta, spawn);
   fprintf(stderr, "colorize progress info\n"); // progress information
   for(i=width*height;i-->0;) {
   	fprintf(stderr, " %d / %d\r", i, width*height); // progress information
      	val16 = fread_uint16(raw_file);
      	if (val16==max16) {
		fputc(inset[0], ppm_file);
		fputc(inset[1], ppm_file);
		fputc(inset[2], ppm_file);
      		} 
      		else map16(val16, ppm_file);
    }



  /*

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
  
  
  
*/
  // end 
  fclose(ppm_file); fprintf(stderr, "file %s is closed \n", ppm_filename);
  fclose(raw_file); fprintf(stderr, "file %s is closed for the second time\n", raw_filename);
  FREE(data);

  exit(0);
  
  return 0;
  
}
