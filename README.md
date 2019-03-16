# Operating-Systems-Term-Project

### Here are some basic git commands
git pull

git add . 
	(adds all the changes to push)

git commit -m "Any message here"

git push


### Helpfull Links

###### Wiki Page

https://en.wikipedia.org/wiki/Ext2

###### Some Random Professors Slides

https://perl.plover.com/classes/ext2fs/

##### VDI File Link

https://forums.virtualbox.org/viewtopic.php?t=8046

##### how read() works
http://pubs.opengroup.org/onlinepubs/000095399/functions/read.html

##### Struct Packing
For example if theres a struct like this:
struct example {
	short num1;
	int num2;
}
In compacted binary it would save 2 bytes for the short and 4 bytes for the int. 
But in c++ it would treat it as 2 bytes for the short but then a 2 byte space so packing is needed to read in condensed data. (So a waste field will be needed in the middle of our struct for those 4 wasted bytes too)
Heres a pretty good link that explains it:
http://www.catb.org/esr/structure-packing/
http://www.kdab.com/~volker/akademy/2015/packing_structs_optimizing_memory_layouts_of_cxx_data_structures.pdf
great resource for the code
https://gcc.gnu.org/onlinedocs/gcc/Structure-Layout-Pragmas.html