# Boggle
Boggle solver in C++
Heavily commented. But it uses a Trie structure to read in a dictionary and then we search that trie structure for words. 
The idea is that you build a trie from a given dictionary and once you have that built you start looking for words given an input BoggleSquare. It will traverse the trie and only search letters if there are children in the trie so it cuts down on that versus a hashtable. 


Using OSX dictionary and a 100 x 100 random string:

## Compile directions: 
```sh 
$g++ -std=c++11 -O3 main.cpp
```

## Times
It's an XCode project but g++ with optimization is faster. That's for sure.



Using a '4x4' standard boggle board:

>Searched 235886 words, found 9 words and it took 0.004809 seconds

Using a '100x100' boggle board:

>Searched 235886 words, found 16958 words and it took 37.959188 seconds


Okay and just for fun I did a '250x250'

>Searched 235886 words, found 38019 words and it took 643.747588 seconds





