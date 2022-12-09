


Fractals in polar coordinate. 

It is fork of [the original code by Mika Seppä]( http://neuro.hut.fi/~mseppa/images/Fract.html)




# Description

Original image (FractExample5.png)

![FractExample5.png](/png/FractExample5.png "FractExample1")

Modified images 
* crop = Cutout = cut ( defining a rectangle in pixels and choose only this rectanle, removing the other parts of an image). Why : because most of image is not interesting exterior of Mandelrots/Julia sets

Here is standard c plane ( flat image = rectangle from Cartesioan plane) with marked part of the plane which is showed in the polar image 

![example5_flat](/png/example5_flat.png "flat")


Mandelbrot set ( polar images)


Image ( example5) 

![FractExample5.png](/png/example5_.png "FractExample5")

```
xSize=8000
ySize=800
./pfract -x 0.160211 -y -0.684246 -size $xSize $ySize -iterations 256 -rad -10 example5.raw
./colorize example5.raw example5.ppm -spawn -400 -scale 1.6 -fadedepth 25
```




New image ( example 7): only one minibrot from above image, more distorted
 
![example7.png](/png/example7.png "example7")


```
xSize=8000
ySize=800
./pfract -x 0.360211 -y -0.684246 -size $xSize $ySize -iterations 256 -rad -10 example7.raw
./colorize example7.raw example7.ppm -spawn -400 -scale 1.6 -fadedepth 25
```


New image ( example 8): more cetered zoom, different diatorsion of decorations


![example8.png](/png/example8.png "example8")

```
xSize=8000
ySize=800
./pfract -x 0.360211 -y -0.57 -size $xSize $ySize -iterations 256 -rad -10 example8.raw
./colorize example8.raw example8.ppm -spawn -400 -scale 1.6 -fadedepth 25
convert example8.ppm example8.png
# crop the image with shotwell
```

![e9.png](/png/e9.png "e9")

```
./pfract -x -1.7864402555636389  -y 0 -size $xSize $ySize -iterations 256 -rad -20 e9.raw
./colorize e9.raw e9.ppm -spawn -400 -scale 1.6 -fadedepth 25
convert e9.ppm e9.png
# crop the image with shotwell
```

![1.732000000000000.png](/png/1.732000000000000.png "1.732000000000000.png")

![1.749694500000000.png](/png/1.749694500000000.png "1.749694500000000.png")




Julia sets ( polar images)

![FractExample1.png](/png/example1_.png "FractExample1")

![FractExample2.png](/png/example2_.png "FractExample2")

![FractExample3.png](/png/example3_.png "FractExample3")

![FractExample4.png](/png/example4_.png "FractExample4")

![FractExample6.png](/png/example6_.png "FractExample6")



# Exponential map and polar coordinate
* [wikibooks](https://en.wikibooks.org/wiki/Fractals/Computer_graphic_techniques/2D/exp)


# Compilation and execution of the program

[My version of the program](./src) can be compiled and run with simple 

```
make
```

Detailes are in 
* Bash script [d.sh](./src/d.sh)
* [Makefile](./src/Makefile)






Original code execution: 

```
pfract -julia -cx -1.115313 -cy -0.304872 -x -0.147394 -y -0.234163 -size 1600 160 -iterations 164 -rad -8 example1.raw
colorize example1.raw example1.ppm -fadedepth 20 -scale 1.4
```



# Files

**Source code**
* origonal , non modified files are in [src/original directory](./src/original)
* my ( modified) files are in [src directory](./src)

**Images = png files** are in [png directory](./png)

**Documantation** is in the [doc directory](./doc)


Raw file specification
* binary file
* contains data

``` 
 0- 7 : Magic number "ITERFILE"
 8- 9 : Image width
10-11 : Image height
12-13 : Number of bytes per iteration value (NBYTES)

IF NBYTES=2:
14-15 : Maximum iterations
16-   : Data (2 bytes per iteration value)

IF NBYTES=4:
14-17 : Maximum iterations
18-   : Data (4 bytes per iteration value)
```


# To do
* show on normal plane what part of that plane is transformed

# Git

create a new repository on the command line
```
echo "# PFRACT" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin git@github.com:adammaj1/PFRACT.git
git push -u origin main
```


## Repo

Change:
* in general settings
  * add Social Preview Image ( Images should be at least 640×320px (1280×640px for best display))
* in repository details ( near About) add
  * description
  * website 
  * Topics (separate with spaces) 
  

Local repository

```
/Dokumenty/algorithm/pfract
```





## Subdirectory

```git
mkdir png
git add *.png
git mv  *.png ./png
git commit -m "move"
git push -u origin main
```
then link the images:

```txt
![](./png/n.png "description") 

```

to overwrite

```
git mv -f 
```



## Github
* [GitHub Flavored Markdown Spec](https://github.github.com/gfm/)
* [md cheat sheet](http://mdcheatsheet.com/)
* [CommonMark Spec](https://spec.commonmark.org)
* [Markdown parser ](https://markdown-it.github.io/)

