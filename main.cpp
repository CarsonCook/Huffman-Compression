//Huffman Compression
//(c) Carson Cook completed June 19/2015

//can be edited to imporve speed...too many for loops going through entire file size
//^^easy way is in getToDecompressFile...use char array read in instead re-putting it into string

#include "Huffman_Compression.h"
#include <fstream>

using namespace std;

void printTree(Node *root)
{
    if (root==NULL)
    {
        return;
    }
    printTree(root->left);
    if (root->parent==NULL)
    {
        cout << "ROOT";
    }
    cout << "NODE: " << root << " LEFT: " << root->left << " RIGHT: " << root->right << " FREQ: " << root->freq << " CHAR: " << root->character << endl;
    printTree(root->right);
}

int main()
{
    //get text file into one string with proper characters
    string toBeCompressed=getToCompressTextFile();
    if (toBeCompressed=="Can't open the file") //flags that file can't be opened, so end program
    {
        return -1;
    }
    //get frequency of each character
    int characters[128];
    for (int i=0; i<128; i++) //initalize as 0
    {
        characters[i]=0;
    }
    for (int i=0; i<(int)toBeCompressed.length(); i++)
    {
        characters[toBeCompressed[i]]++;
    }
    //make queue of characters, lowest freq first
    bool listStarted=false; //indicates if have initalized first node in queue
    Node *start=new Node; //node starts list
    for (int i=0; i<128; i++)
    {
        if (characters[i]>0) //no point in adding characters that never appear to queue
        {
            if(!listStarted) //init queue
            {
                initNode(start,true,characters[i],(char)i);
                listStarted=true; //only start list once
            }
            else
            {
                addListNode(&start,true,characters[i],(char)i);
            }
        }
    }
    //merge first two nodes into tree, add frequencies to make parent, smaller freq is parent->left, bigger is right, then put tree back into list
    mergeList(&start);
    string codes[128];
    string oneCode; //placeholder for htis call, becauseuse with recursion to call within function
    getLetterCodes(start,codes,oneCode);
    //now make string with 0s and 1s instead of letters
    string binary;
    for (int i=0; i<(int)toBeCompressed.length(); i++)
    {
        binary+=codes[toBeCompressed[i]]; //adds code for each letter in toBeCompressed
    }
    int padding;
    string dec;
    binaryToTextDec(binary,dec,padding); //get back how many 0s added so can say in text file how many bits are just added and not real info
    printToFile(dec,padding,codes);

    //DONE COMPRESSION
    //--------------------------------------------------------------------------------
    //NOW DOING DECOMPRESSION

    string toBeDecompressed=getToDecompressTextFile();
    if (toBeDecompressed=="Can't open the file") //flag that file wasn't opened
    {
        return -1; //end program before attempting to manipulate string that is NULL
    }
    //now parse file into getting padding, characters and codes and the binary for each character
    string codesFromHeader[128];
    string compressedText=parseCompressedFile(codesFromHeader,toBeDecompressed);
    //now make tree to go through using codes --> 0=left, 1=right, when at leaf print the char and go back to root
    Node *root=new Node; //start root off outside of tree, as 0 and 1 need to point left/right from something at first
    initNode(root,false,0,'a'); //freq just a placeholder from here on out, and 'a' is placeholder since it's not a leaf
    for (int i=0; i<128; i++)
    {
        if (codesFromHeader[i]!="") //if true then the character (char)i wasn't in original file, ignore
        {
            makeTree(&root,codesFromHeader[i],(char)i);
        }
    }
    //have tree, now go through using binary string and decompress
    string originalText=decompress(compressedText,root);
    //have fully decompressed, all there is now is to output to a file
    ofstream reOutFile("Decompressed War and Peace.txt");
    if (!reOutFile.is_open())
    {
        cout << "Can't open \"Decompressed War and Peace.txt\"" << endl;
        return -1;
    }
    reOutFile << originalText;
    return 0;
}
