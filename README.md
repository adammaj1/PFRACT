


FRACTALS IN POLAR COORDINATES, It is fork of original code by Mika Seppä ( http://neuro.hut.fi/~mseppa/images/Fract.html )



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

