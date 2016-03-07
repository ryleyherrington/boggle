#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

vector<string>foundWords;


class Trie{
public:
    Trie* parent;
    bool isWord;
    bool hasBeenAdded;
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
, hasBeenAdded(false)
, letter(0)
{
}

Trie::Trie(Trie *par, char c)
: parent(par)
, isWord(false)
, hasBeenAdded(false)
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
//    printf("\nParent: %c\n", c);
    for (int i=0; i<children.size(); i++){
 //       printf("\t\t%c", children[i]->letter);
        if (children[i]->letter == c) {
            return i;
        }
    }
    return -1;
}

string Trie::finalWord(string currWord) {
    if (letter == '\0'){
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
            return;
        }
        children[idx]->addWord(tail); //delete first char and keep moving down the word
        
    } else { //node doesn't exist
        Trie* t = new Trie(this, word[0]); //create new child
        if (tail.size() == 0) {
            t->isWord = true;
            children.push_back(t); //add child to root
            return;
        }
        children.push_back(t); //add child to root
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
    checkWord = checkWord + b->letter; //old prefix + new letter
    
    if (t->isWord && t->hasBeenAdded == false) { //if it's a word
        t->hasBeenAdded = true;
        if (prefix.length() > 2)
            foundWords.push_back(prefix); //add to found words
    }
    
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
    for (int i=0; i<m; i++) {
        for (int j=0; j<m; j++) {
            traverse(root, "", Pos(i,j), bb, m, n);
        }
    }
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

int main(int argc, char* argv[] )
{
    string inputString = "yoxrbaved";
    vector <string> dict;
//    ifstream inf("dictionary.txt");
    fstream f;
    f.open("/usr/share/dict/words", fstream::in);

    string word;
    while (f >> word) {
        dict.push_back(word);
    }
    
//    vector<string> dictionary = {
//        "bred","yore","byre","abed","oread","bore","orby","robed","phone",
//        "broad","byroad","robe","bored","derby","bade","aero","read", "small",
//        "orbed","verb","aery","bead","bread","very","road", "ryley", "testing"
//    };
    
    Trie* root = new Trie();
//    for (int i=0; i<dictionary.size(); i++) {
    for (int i=0; i<dict.size(); i++) {
        root->addWord(dict[i]);
    }
    
    printTrie(root);
    
    int m = 3; //rows
    int n = 3; //columns
    vector<B_row_t> bb = buildBoard(m, n, inputString); //boggle board
    
//   	vector<string> foundWords = findWords(m, n, bb, dictionary, root);
   	vector<string> foundWords = findWords(m, n, bb, dict, root);
    printf("Found %lu words:\n", foundWords.size());
    for (int j = 0; j < foundWords.size(); ++j) {
        printf("%s\n", foundWords[j].c_str());
    }

    return 0;
}


