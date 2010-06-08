Copy the dll to notepad++ plugin directory.
Create a directory gtagfornplus in notepad++ plugin directory.
Get ctags from https://sourceforge.net/projects/ctags/files/ctags/5.5/ec55w32.zip/download
Get global/gtags from http://patraulea.com/hacks/gtags-win32/gtags-5.7.6.tgz. 
I downloaded the above version of gtags and ctags and it works for me. Am not associated with those projects and am just a user. So please don’t complain if it does not work for you!
Copy the gtags/global binaries and ctags binaries to "gtagfornplus" directory created (in above step).
You might have to add the full directory path of "gtagfornplus" to Windows environment. (Right click "My Computer" -> Advanced -> Environment variables -> System Variables -> select PATH from list item -> press edit)
In a command window, go to your source root.
Run gtags, this should generate the global tag DB.
Run ctags -R, this should generate the ctags tag db.

Plugin Usage:
-------------
Reference search:
-----------------
Open the source file in notepad++ and use ctrl+alt+c for reference search. The files which reference the symbol are listed in first window. 
Double click on an item and the actual references in file gets populated in second list box.
Double click an item on the second list box to open the actual reference.

Definition search:
------------------
Open the source file in notepad++ and use ctrl+alt+b for definition search. The files which define the symbol are listed in first window. 
Double click on the item in list box and the file where the symbol is defined is opened (and cursor points to the place where symbol is defined).

