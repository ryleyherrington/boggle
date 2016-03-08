# boggle
Boggle solver in c++

Using OSX dictionary and a 100 x 100 random string:

Compile directions: g++ -std=c++11 -O3 main.cpp

It's an XCode project but g++ with optimization is faster. That's for sure.

*Searched 235886 words, found 16958 words and it took 37.959188 seconds*


Using a 4x4 standard boggle board:

*Searched 235886 words, found 9 words and it took 0.004809 seconds*

Heavily commented. But it uses a Trie structure to read in a dictionary and then we search that trie structure for words. Pretty fast. 


Okay and just for fun I did a 250x250

*Searched 235886 words, found 38019 words and it took 643.747588 seconds*





