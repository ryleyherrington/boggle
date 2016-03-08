#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

//Instead of passing around a local,
//this global will hold all the words we find
vector<string>foundWords;

//This might be better understood as a Trie Node
class Trie{
public:
    Trie* parent;                   //The node above
    bool isWord;                    //If it's the end of a word, mark this value
    bool hasBeenAdded;              //Has this word been added to our list
    char letter;                    //The letter the node holds
    vector<Trie*> children;         //Trie* children[26];
    Trie();                         //Default constructor
    Trie(Trie* t, char c);          //Real constructor
    void addWord(string word);      //Add word to structure
    string finalWord(string c);     //Figures out word from final letter
    int letterInChildren(char c);   //How many children nodes are below this node
};

//Default constructor
Trie::Trie()
: parent(NULL)
, isWord(false)
, hasBeenAdded(false)
, letter(0)
{
}

//More useful constructor
Trie::Trie(Trie *par, char c)
: parent(par)
, isWord(false)
, hasBeenAdded(false)
, letter(c)
{
}

//This is a struct that will hold info for each letter from the input
struct BoggleSquare{
    bool isUsed; //has it been used in the current traversal
    char letter; //the letter in the hypothetical square
    BoggleSquare(bool x, char y) { isUsed = x; letter = y; }
    
    BoggleSquare()
    : isUsed(false)
    , letter('\0')
    {};
};

//This is a vector of boggle squares which will act as the row
//A vector of these will be an entire board
typedef vector<BoggleSquare> B_row_t; //Boggle row type

//Position struct
struct Pos {
    int row;
    int col;
    Pos(int x, int y) { row = x; col = y; }
};

/*
 * These are offsets from the current Trie node
 * We start by going to the right, and then
 * checking counter clockwise around the node
 *
 * -1,1   0,1  1,1
 *     \   |   /
 * -1,0 -   t   - 1,0
 *      /  |   \
 * -1,-1  0,-1  1,-1
 */
Pos offsets [] = {
    {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}
};

//Returns the index of where a letter is in the children trie vector if it exists
int Trie::letterInChildren(char c) {
    for (int i=0; i<children.size(); i++){
        if (children[i]->letter == c) {
            return i;
        }
    }
    return -1;
}

//Returns a full word string by traversing up the trie structure
string Trie::finalWord(string currWord) {
    if (letter == '\0')
        return currWord;
    
    //prepend
    currWord = letter + currWord;
    return parent->finalWord(currWord);
}

//Adds a word to the trie structure
void Trie::addWord(string word) {
    //Check to see if we already have a child node with that letter
    int idx = this->letterInChildren(word[0]);
    string tail = word.substr(1);
    if (idx != -1 ){ //node already exists, so don't add it to trie
        if (tail.size() == 0) {
            this->isWord = true; //set the word flag if there aren't any more letters
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

//Builds the boggle board from the size given and the input string
//m = rows
//m = columns
vector<B_row_t> buildBoard(int m, int n, string inputString) {
    int currPos = 0;
    vector<B_row_t> bb;
    //loop through rows
    for (int i = 0; i < m; i++) {
        B_row_t newRow; //create each new row
        for (int j = 0; j < n; j++) {
            //for each character make a new square and add it for each column
            BoggleSquare b(false, inputString[currPos]); //create it on stack
            newRow.push_back(b);
            currPos++;
        }
        bb.push_back(newRow);
    }
    return bb;
}

/* Main algorithm
 *
 * The main idea of this is that given any node check all of the neighbors
 * and see if that's a word. If it is then add it, otherwise you recursively
 * call down on each of the neighbors and keep going until you've finished finding
 * all of the words given the inital Trie* t.
*/
void traverse(Trie *t, string prefix, Pos p, vector<B_row_t>bb, int rowsize, int colsize) {
    int row = p.row;
    int col = p.col;
    
    if ((row < 0) || (row >= rowsize) || (col < 0) || (col >= colsize)) //check boundaries
        return;
    
    BoggleSquare *b = &bb[row][col]; //get current square
    if (b -> isUsed) //if used, move on
        return;
    
    
    if (t->isWord && t->hasBeenAdded == false) { //if it's a word
        t->hasBeenAdded = true;
        if (prefix.length() > 2)
            foundWords.push_back(prefix); //add to found words
    }
    
    string checkWord(prefix + b->letter);//old prefix + new letter
    
    b->isUsed = true;
    for (int i=0; i<8; i++) { //This will check each of the neighboars via offsets
        if (t->children.size() > 0) {
            int idx = t->letterInChildren(b->letter); //Check to see if there are children in Trie structure
            if (idx != -1) { //meaning there is a child
                traverse(t->children[idx], checkWord, Pos(row + offsets[i].row, col+offsets[i].col), bb, rowsize, colsize); //recursive call
            }
        }
    }
    b->isUsed = false;
}

//Find Words is the function that fills the requirements given
//Loop through each row and column and find all words in traverse
//m is # of rows
//n is # of columns
vector<string>findWords(int m, int n, vector<B_row_t>bb, vector<string>dictionary, Trie *root) {
    for (int i=0; i<m; i++) {
        for (int j=0; j<n; j++) {
            traverse(root, "", Pos(i,j), bb, m, n);
        }
    }
    return foundWords;
}

//Use indentation to display trie structure
void printTrie(Trie* t, int indentLevel) {
    if (t->letter == '\0')
        printf("%*cRoot of trie\t", indentLevel, ' ');
    else
        printf("%*c \t", indentLevel, t->letter);
    
    // If there are any "children", write them out with a label
    if (t->children.size()){
        if (t->children[0])
            printf("Next possible characters: ");
        for (int i=0; i < t->children.size(); i++) {
            if (t->children[i])
                printf("%c", t->children[i]->letter);
        }
    }
    
    // Write out other state
    if (t->isWord)
        printf("\tisWord");
    printf("\n");
    
    for (int i=0; i < t->children.size(); i++) {
        if (t->children[i])
            printTrie(t->children[i], indentLevel+2);
    }
}

int main(int argc, char* argv[] )
{
    int m = 4; //rows
    int n = 4; //columns
    
    //Create a random input string for our board
    string inputString = "";
	srand(time(0)); //initializes the call to rand to be better at psuedo randomness

    //comment the next 2 lines out if you want to provide your own string
    for (int s=0; s<m*n; s++)
        inputString += "abcdefghijklmnopqrstuvwxyz"[rand() % 26];
    printf("Input string:%s\n", inputString.c_str());
    
    //Choose a dictionary - I chose the system dictionary on OSX
    vector <string> dict;
    fstream f;
    f.open("/usr/share/dict/words", fstream::in);
    
    string word;
    while (f >> word) {
        //for each word add it to our local dictionary
        dict.push_back(word);
    }
    
    //Create our Trie structure by adding each word from our dictionary
    Trie* root = new Trie();
    for (int i=0; i<dict.size(); i++) {
        root->addWord(dict[i]); //parses the word into our structure
    }
    
    //Creates our boggle board
    vector<B_row_t> bb = buildBoard(m, n, inputString);
    
    //This is just for fun, but it's nice to know
    clock_t start;
    double duration;
    
    start = clock();
   	vector<string> foundWords = findWords(m, n, bb, dict, root); //Main call- find the words in the boggle board
    duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
    printf("Searched %lu words, found %lu words and it took %f seconds\n", dict.size(), foundWords.size(), duration);
    
    //If you want to print out the words found, uncomment these lines
    //for (int j = 0; j < foundWords.size(); j++)
    //    printf("%s\n", foundWords[j].c_str());

    return 0;
}


