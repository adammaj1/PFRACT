/*

gcc p.c -lm -Wall -Wextra 
--------------------------------

Fractals in polar coordinates by Mika Seppä : neuro.hut.fi/~mseppa/images/Fract.html 
"I have written two programs to calculate Mandelbrot ... in polar coordinates. 
 
The coordinate system is such that the angular component goes to the y-axis, and the radius
goes to the x-axis of the resulting image. In addition, the x-axis (radius) is normalized with exp-function so that angles are preserved in this mapping. Here is the list of available options:


My changes:
* convert 2 files into 1 file
* remove raw file
* remove dead code 
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> // strcmp



#define FLOAT double
#define EXPF  exp
#define POWF  pow
#define SINF  sin
#define COSF  cos
#define FLOOR(xx) floor(xx)
#define UINT16     unsigned short
#define IVAL        UINT16
#define MALLOC(nn,tt) (tt *)malloc((nn)*sizeof(tt))
#define FREE(dd) free(dd)
#define UNIT2DEG(xx) ((xx)*360.0)

static UINT16 map16_maxiter = 1, map16_fo = 0;
static FLOAT  map_cscale = 1.0, map_mulf = 1.0, map_muls = 1.0/20.0;
static FLOAT  map_starta=0.0, map_spawn = 1.0;

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



void fill_raw_array_mandel(IVAL *data, FLOAT *origin, FLOAT *radius, int *size, IVAL maxiter) { 
	
  FLOAT f, sf, cf, mulx, muly;
  FLOAT cx;
  FLOAT cy;
  IVAL  i;
  int   x, y;
  
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


int MakeImage(FLOAT c0x){

  FLOAT   origin[2] = {  c0x, 0.0 }; // c0 
   FLOAT   radius[2] = { -20.0, 2.0 };
  int     size[2]   = {  2400, 800 }; // Image width, Image height
  FILE  *ppm_file = stdout;
  char name [100]; /* name of file */
  IVAL    maxiter = 20000;
  int i;
  IVAL   *data = NULL;
  UINT16         width;
  UINT16 height;
  int length;
  UINT16 val16;
  UINT16 max16;
  FLOAT          cscale = 1.5; 
  FLOAT starta = 45.0; // -start  	: Starting color angle (0)
  FLOAT  spawn = - 360.0; // -spawn  	: Color angle to spawn (360)
  int            fadeout=0;
  int  fadedepth=100;
  unsigned char  inset[3] = { 0, 0, 0 }; // color of the M-set , usually black
  
  
  // setup 
  data = MALLOC(size[0]*size[1], IVAL);
  radius[1] = radius[0] + (FLOAT)size[0]/(FLOAT)size[1] * 2.0 * 3.14159265;
  width  = size[0]; 
 height = size[1]; 
 length = width*height;
 max16 = maxiter; //
 
   // fill data array
  fill_raw_array_mandel(data, origin, radius, size, maxiter); // , cscale
  
  // find the color of the pixel and save it to the ppm file
  // binary Portable PixMap P6 with .ppm extension
  // open file for writing to it
  snprintf(name, sizeof name, "%.16f", fabs(origin[0])); /*  */
  char *ppm_filename =strcat(name,".ppm");
  ppm_file = fopen(ppm_filename, "wb");
  if (ppm_file==NULL) 
  	{ fprintf(stderr, "Cannot open file %s \n", ppm_filename); return 4;}
  	else { fprintf(stderr, "I can open file %s \n", ppm_filename);}

    fprintf(ppm_file, "P6\n%d %d\n255\n", width, height); // header
  
    // map iteration data to color
   // read from data array not raw file 
   map16_init(max16, cscale, fadeout, fadedepth, starta, spawn);
   fprintf(stderr, "colorize progress info\n"); // progress information
   for(i=0;i< length;i++){
   //for(i= length;i-->0;) {
   	fprintf(stderr, " %d / %d\r", i, length); // progress information
      	val16 = data[i]; //fread_uint16(raw_file);
      	if (val16==max16) {
		fputc(inset[0], ppm_file);
		fputc(inset[1], ppm_file);
		fputc(inset[2], ppm_file);
      		} 
      		else map16(val16, ppm_file);
    }


  // end 
  fclose(ppm_file); fprintf(stderr, "file %s is closed \n", ppm_filename);
  //fclose(raw_file); fprintf(stderr, "file %s is closed for the second time\n", raw_filename);
  FREE(data);
  
  // info
    fprintf(stderr,"------------------------------------------------------------\n");
    fprintf(stderr,"|                 Iteration file colorizer                 |\n");
    fprintf(stderr,"|    (C) Mika Seppa 1996 (http://neuro.hut.fi/~mseppa/)    |\n");
    fprintf(stderr,"------------------------------------------------------------\n");
    fprintf(stderr," -fadeout ...     : Fades out to white (%d)\n", fadeout);
    fprintf(stderr," -fadedepth ...   : Depth of fading (%d)\n", fadedepth);
    fprintf(stderr," -help            : This help\n");
    fprintf(stderr," -info            : Show information about file\n");
    fprintf(stderr," -inset .. .. ..  : Color in the set (%d,%d,%d)\n",inset[0], inset[1], inset[2]);
    fprintf(stderr," -scale ...       : Color scale (%g)\n", cscale);
    fprintf(stderr," -spawn ...       : Color angle to spawn (%.0f)\n", UNIT2DEG(spawn));
    fprintf(stderr," -start ...       : Starting color angle (%.0f)\n", UNIT2DEG(starta));
    fprintf(stdout,"c0 = %.16f %+.16f it is origin here\n", origin[0], origin[1]);
  fprintf(stdout,"radius minimal = %.16f  \t radius =%+.16f \n", radius[0], radius[1]); 
  fprintf(stdout,"Image width = %d \t Image height = %d\n", size[0], size[1]);
  fprintf(stdout,"Maximum iterations = maxiter = %d \n", maxiter);
  fprintf(stdout, "2-byte file: size %dx%d, maxiter %d\n", width, height, max16);
  
  exit(0);
}

int  main(void ) {

	FLOAT c0x = -1.786440255563638;
    MakeImage(c0x);
   return 0;
  
}
