


Fractals in polar coordinate. 

It is fork of [the original code by Mika Seppä]( http://neuro.hut.fi/~mseppa/images/Fract.html)

# Original description


![FractExample1.png](/png/FractExample1.png "FractExample1")

![FractExample2.png](/png/FractExample2.png "FractExample2")

![FractExample3.png](/png/FractExample3.png "FractExample3")

![FractExample4.png](/png/FractExample4.png "FractExample4")

![FractExample5.png](/png/FractExample5.png "FractExample5")

![FractExample6.png](/png/FractExample6.png "FractExample6")


# Compilation and execution of the program

[My version of the program](./src) can be compiled and run with simple 

```
make
```

Detailes are in 
* Nash script [d.sh](./src/d.sh)
* [Makefile](./src/Makefile)


**The problem** : my problem compiles, run, creates images, but they are all white .... 



Original code execution: 

```
pfract -julia -cx -1.115313 -cy -0.304872 -x -0.147394 -y -0.234163 -size 1600 160 -iterations 164 -rad -8 example1.raw
colorize example1.raw example1.ppm -fadedepth 20 -scale 1.4
```



# Files

** Source code**
* origonal , non modified files are in [src/original directory](./src/original)
* my ( modified) files are in [src directory](./src)

** Images = png files** are in [png directory](./png)

** Documantation ** is in the [doc directory](./doc)


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

