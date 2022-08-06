# microemacs
## My microemacs <h2> tag

**In Windows:**

For Visual Studio 2006:

cd wincon/release

gmake

The executable e.exe is also available here.

For Visual Studio 2019:

Open Project/Solution vs2019/memacs.sln

**In Linux**

cd unix

make

This makes the executable microemacs.

**Installation**

Put the executable, (macros/emacs.rc or macros/.emacsrc), src/emacs.md
in a directory on the path.

###Examples <h3>

 * dir | e
 * e .
 * e README.md
 * e guest@computer:shared/file.txt
 * e -b.mm README.md
 * demote-buffer
 * fetch-file
 * word-search
 * goto-matching-fence
 * search-incls
 * find-tag


