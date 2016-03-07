/*
 Write a function which will find all the words on a generalized Boggle™ board.  The function should take as input the board dimensions, the board, and a dictionary of valid words and it should output the list of all found words to the calling function. You should consider how your approach will perform with a large dictionary and a large board.
 
 If you've never heard of Boggle then see http://en.wikipedia.org/wiki/Boggle for a better description than I can write.  A 3x3 boggle board that looks like this:
 
	y o x
	r b a
	v e d
 
 Has exactly the following words on it (according to my program):
 
	bred, yore, byre, abed, oread, bore, orby, robed, broad, byroad, robe
	bored, derby, bade, aero, read, orbed, verb, aery, bead, bread, very, road
 
 Note that it doesn't have “robbed” or “robber” because that would require reusing some letters to form the word.  And it doesn’t have “board” or “dove” because that would require using letters which aren’t neighbors.
 
 */

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

vector<string>foundWords;


class Trie{
public:
    Trie* parent;
    bool isWord;
    char letter;
    vector<Trie*> children; //Trie* children[26];
    Trie();
    Trie(Trie* t, char c);
    void addWord(string word);
    string finalWord(string currWord);
    int letterInChildren(char c);
};

Trie::Trie()
: parent(NULL)
, isWord(false)
, letter(0)
{
}

Trie::Trie(Trie *par, char c)
: parent(par)
, isWord(false)
, letter(c)
{
}

struct BoggleSquare{
    bool isUsed;
    char letter;
    BoggleSquare(bool x, char y) { isUsed = x; letter = y; }
    
    //BoggleSquare(){isUsed=false; letter='\0';};
    BoggleSquare()
    : isUsed(false)
	   , letter('\0')
    {};
};

typedef vector<BoggleSquare> B_row_t; //Boggle row type

struct Pos {
    int row;
    int col;
    Pos(int x, int y) { row = x; col = y; }
};

// struct offset_t; //forward declaration

struct offset_t {
    int row_offset;
    int col_offset;
};

offset_t offsets [] = {
    {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}
};

int Trie::letterInChildren(char c) {
    //    printf("letter:'%c' \n",c);
    for (int i=0; i<children.size(); i++){
        if (children[i]->letter == c) {
            return i;
        }
    }
    return -1;
}

string Trie::finalWord(string currWord) {
    if (letter == '\0'){
        printf("%s\n",currWord.c_str());
        return currWord;
    }
    
    //prepend
    //currWord.insert(0, letter);
    currWord = letter + currWord;
    return parent->finalWord(currWord);
}

void Trie::addWord(string word) {
    int idx = this->letterInChildren(word[0]);
    string tail = word.substr(1);
    if (idx != -1 ){ //node already exists
        if (tail.size() == 0) {
            this->isWord = true;
            printf("%c\n", word[0]);
            return;
        }
        children[idx]->addWord(tail); //delete first char and keep moving down the word
        
    } else { //node doesn't exist
        Trie* t = new Trie(this, word[0]); //create new child
        children.push_back(t); //add child to root
        if (tail.size() == 0) {
            t->isWord = true;
            printf("%c\n", word[0]);
            return;
        }
        t->addWord(tail); //move down the word
    }
}

vector<B_row_t> buildBoard(int m, int n, string inputString) {
    int currPos = 0;
    vector<B_row_t> bb;
    for (int i = 0; i < m; i++) {
        B_row_t newRow;
        for (int j = 0; j < n; j++) {
            //BoggleSquare *b = new BoggleSquare(false, inputString[currPos]); // heap with values
            BoggleSquare b(false, inputString[currPos]); //on stack object  with values
            newRow.push_back(b);
            currPos++;
        }
        bb.push_back(newRow);
    }
    return bb;
}

void traverse(Trie *t, string prefix, Pos p, vector<B_row_t>bb, int rowsize, int colsize) {
    int row = p.row;
    int col = p.col;
    
    if ((row < 0) || (row >= rowsize) || (col < 0) || (col >= colsize)) //check boundaries
        return;
    
    BoggleSquare *b = &bb[row][col]; //get current square
    if (b -> isUsed) //if used, move on
        return;
    
    string checkWord(prefix);
    //checkWord.append(b.letter); //old prefix + new letter
    checkWord = checkWord + b->letter; //old prefix + new letter
    
    if (t->isWord) //if it's a word
        foundWords.push_back(checkWord); //add to found words
    
    b->isUsed = true;
    for (int i=0; i<8; i++) {
        if (t->children.size() > 0) {
            int idx = t->letterInChildren(b->letter);
            if (idx != -1) {
                traverse(t->children[idx], checkWord, Pos(row + offsets[i].row_offset, col+offsets[i].col_offset), bb, rowsize, colsize);
            }
        }
    }
    b->isUsed = false;
}

//m is # of rows
//n is # of columns
vector<string>findWords(int m, int n, vector<B_row_t>bb, vector<string>dictionary, Trie *root) {
    traverse(root, "", Pos(0,0), bb, m, n);
    return foundWords;
}

void printTrie(Trie* t) {
    for (int i=0; i < t->children.size(); i++) {
        if (t->children[i] != NULL) {
            //printf("%d, '%c'\n", i, t->children[i]->letter);
            printTrie(t->children[i]);
        }
    }
}

int main()
{
    string inputString = "yoxrbaved";
    
//    vector<string> dictionary = {
//        "bred","yore","byre","abed","oread","bore","orby","robed","phone",
//        "broad","byroad","robe","bored","derby","bade","aero","read", "small",
//        "orbed","verb","aery","bead","bread","very","road", "ryley", "testing"
//    };
    
    vector<string> dictionary = {
        "yore", "bore", "ryley"
    };
    //printf("starting\n");
    Trie* root = new Trie();
    for (int i=0; i<dictionary.size(); i++) {
        //printf("%s\n", dictionary[i].c_str());
        root->addWord(dictionary[i]);
    }
    
    printTrie(root);
    
    int m = 3;
    int n = 3;
    vector<B_row_t> bb = buildBoard(m, n, inputString); //boggle board
    
   	vector<string> foundWords = findWords(m, n, bb, dictionary, root);
    printf("Found words:\n");
    
}


