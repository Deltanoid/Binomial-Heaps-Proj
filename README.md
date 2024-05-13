# Binomial-Heaps-Proj
Project code for the Design &amp; Analysis of Algorithms course

## Goal:
Implementing insert and delete-min for binomial heaps on cpu for single-threading then supporting multi-threading. Comparing the speed up then trying to implement optimizations.

## What Works:
- [main.cpp](main.cpp) contains the fully working fully threaded implementation. A compiled version can be found inside the output folder. <br>
- [randGen.py](randGen.py) can be manually edited to output a file with the desired number of unique keys to use for testing with the heap. The keys have a random offset from each other and are strictly increasing.<br>
- [main_multi.cpp](main_multi.cpp) has most of the same code as the single threaded but the union function has been replaced with phase I & II but is still missing prefix-minima & phase III. Compiling this code will do nothing.

## Notes:
For the visualization to work you will need to install [Graphviz](https://graphviz.org/) and be using Windows. <br>
Instructions for the commands appear when you run the file.
