#ifndef HUFFMAN_COMPRESSION_H
#define HUFFMAN_COMPRESSION_H

#include <iostream>
#include <string>

using namespace std;

string getToCompressTextFile(void);//gets "War and Peace.txt" in a string, with non-ASCII characters removed
string getToDecompressTextFile(void); //gets "Compressed War and Peace.comp"

struct Node //node in tree/queue
{
    char character;
    int freq;
    bool leaf;
    //node for queue
    Node *next;
    //nodes for tree
    Node *left;
    Node *right;
    Node *parent;
};

void initNode(Node *newNode,bool leaf,int freq,char character); //inits node
void addListNode(Node **start,bool leaf,int freq,char character); //adds node to queue (not front)
void addNodeFront(Node *newNode,Node **start); //adds node to front of queue
void mergeList(Node **start);//merge first two nodes into tree, add frequencies to make parent, smaller freq is parent->left, bigger is right, then put tree back into list
void addTreeNode(Node **start,Node *newRoot); //adds root of each tree made into queue for next merge

void getLetterCodes(Node *root,string codes[],string oneCode);//traverses entire tree and gets code for each letter

void binaryToTextDec(string binary,string &dec,int &padding); //changes binary string to decimal string, and adds enough 0s to be divisible by 64
int binaryTo10(int binary); //coverts integer binary to integer deciaml

void printToFile(string dec,int padding,string codes[]); //outputs compressed (decimal) text to a file (compressed War and Peace.comp)

string parseCompressedFile(string codes[],string toBeDecompressed); //gets the binary of the integers in char form with padding removed and the codes for each character from the header
string asciiToBinary(string compressedText); //converts ascii to binary
int decimalToBin(int ascii); //converts decimal to binary

void makeTree(Node **root,string code,char character); //creates decompression tree
string decompress(string compressedText,Node *root); //follows through tree and decompresses text
#endif // HUFFMAN_COMPRESSION_H
