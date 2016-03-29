#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Huffman_Compression.h"

string getToCompressTextFile(void)
{
    ifstream inFile("War and Peace.txt");
    //check if file can be opened, if not end program
    if (!inFile.is_open())
    {
        cout << "Can't open \"War and Peace.txt\"" << endl;
        return "Can't open the file";
    }
    string lineFromFile;
    string toBeCompressed;
    toBeCompressed={istreambuf_iterator<char>(inFile), istreambuf_iterator<char>() };
    inFile.close();
    //remove non-ASCII characters (taken from unicode source)
    for (int i=0; i<(int)toBeCompressed.length(); i++)
    {
        if (!isprint(toBeCompressed[i]) and toBeCompressed[i]!='\n')
        {
            toBeCompressed.erase(i,1);
        }
    }
    return toBeCompressed;
}

string getToDecompressTextFile(void)
{
    ifstream inFile("Compressed War and Peace.comp");
    //check if file opened and if not say so and return a value that wil flag we couldn't open it
    if(!inFile.is_open())
    {
        cout << "Can't open \"Compressed War and Peace.comp\"" << endl;
        return "Can't open the file";
    }
    //read in file
    inFile.seekg (0, inFile.end); //gets position of last char in file
    int fileSize=inFile.tellg(); //save last char position
    inFile.seekg(0,ios::beg); //go back to first char position
    char *memblock = new char [fileSize]; //create space to read file into
    inFile.read (memblock, fileSize); //reads file into memory space
    string toBeDecompressed;
    for (int i=0; i<fileSize; i++) //take array of chars and put into string
    {
        toBeDecompressed+=memblock[i];
    }
    inFile.close();
    delete[] memblock;
    return toBeDecompressed;
}

void initNode(Node *newNode,bool leaf,int freq,char character)
{
    newNode->freq=freq;
    newNode->leaf=leaf;
    if (leaf)
    {
        newNode->character=character;
    }
    else
    {
        newNode->character='a'; //placeholder char
    }
    newNode->left=NULL;
    newNode->right=NULL;
    newNode->parent=NULL;
    newNode->next=NULL;
}

void addListNode(Node **start,bool leaf,int freq,char character)
{
    Node *newNode=new Node;
    initNode(newNode,leaf,freq,character);
    if (newNode->freq<(*start)->freq)
    {
        Node *temp=*start; //use as start value if need to add to front, so start is actually changed
        addNodeFront(newNode,&temp);
        *start=temp;
    }
    else
    {
        bool addedNode=false; //remebers if node gets inserted
        Node *current=*start;
        while (current->next!=NULL)
        {
            if ((current->next)->freq>newNode->freq)
            {
                newNode->next=current->next;
                current->next=newNode;
                addedNode=true;
                break;
            }
            current=current->next; //move through list
        }
        if (!addedNode) //means newNode is last item in queue
        {
            current->next=newNode;
        }
    }
}

void addNodeFront(Node *newNode,Node **start)
{
    newNode->next=*start;
    *start=newNode; //new start of list is newNode
}

void mergeList(Node **start)
{
    while (((*start)->next)->next!=NULL) //go until two nodes left in list
    {
        //make tree for 2 elements
        Node *newRoot=new Node;
        initNode(newRoot,false,(*start)->freq+((*start)->next)->freq,'a'); //a is placeholder, not actually setting a character because its not a leaf
        //hook up connections in tree
        if ((*start)->freq<((*start)->next)->freq)
        {
            newRoot->left=*start;
            newRoot->right=(*start)->next;
        }
        else
        {
            newRoot->left=(*start)->next;
            newRoot->right=*start;
        }
        (*start)->parent=newRoot;
        ((*start)->next)->parent=newRoot;
        Node *temp=((*start)->next)->next; //save new starting node in list
        //break connections in list for start and start->next, just to be tidy with connections
        ((*start)->next)->next=NULL;
        (*start)->next=NULL;
        addTreeNode(&temp,newRoot);//insert tree root back into list
        *start=temp;
    }
    //now have 2 items in list, so just merge the 2, result is final root for tree
    Node *root=new Node;
    initNode(root,false,(*start)->freq+((*start)->next)->freq,'a'); //a is placeholder, not actually setting a character because its not a leaf
    //hook up connections in tree
    ((*start)->next)->parent=root;
    (*start)->parent=root;
    if ((*start)->freq<((*start)->next)->freq)
    {
        root->left=*start;
        root->right=(*start)->next;
    }
    else
    {
        root->left=(*start)->next;
        root->right=*start;
    }
    *start=root;
}

void addTreeNode(Node **start,Node *newRoot)
{
    if (newRoot->freq<(*start)->freq)
    {
        Node *temp=*start; //use as start value if need to add to front, so start is actually changed
        addNodeFront(newRoot,&temp);
        *start=temp;
    }
    else
    {
        bool addedNode=false; //remebers if node gets inserted
        Node *current=*start;
        while (current->next!=NULL)
        {
            if ((current->next)->freq>newRoot->freq)
            {
                newRoot->next=current->next;
                current->next=newRoot;
                addedNode=true;
                break;
            }
            current=current->next; //move through list
        }
        if (!addedNode) //means newNode is last item in queue
        {
            current->next=newRoot;
        }
    }
}

void getLetterCodes(Node *root,string codes[],string oneCode)
{
    if (root->left!=NULL)
    {
        oneCode+="0"; //went left, add 0 to code
        getLetterCodes(root->left,codes,oneCode);
    }
    if (root->right!=NULL)
    {
        oneCode.erase(oneCode.length()-1,1); //remove last 0 in code, that's from going left at parent instead of right
        oneCode+="1";//went right, add 1 to code
        getLetterCodes(root->right,codes,oneCode);
    }
    if (root->leaf) //only leafs have real letters we want codes for
    {
        codes[root->character]=oneCode;
    }
}

void binaryToTextDec(string binary,string &dec,int &padding)
{
    //add 0s to end of binary to make it divisible into 8 bit chunks
    padding=8-(int)binary.length()%8;
    for (int i=0; i<padding; i++)
    {
        binary+="0";
    }
    int i=0; //init here for for loop so that keep going by chunks of 8 and don't always start from i=0
    while (i<binary.length()-1)//turns all binary to dec
    {
        //take 8 0s and 1s from binary
        string eightBits;
        int j=i;
        for (i; i<j+8; i++)
        {
            eightBits+=binary[i];
        }
        //ocnvert string to int
        stringstream binStringToInt;
        binStringToInt << eightBits;
        int bin;
        binStringToInt >> bin;
        int decimal=binaryTo10(bin);
        dec+=(char)decimal;//add ascii character that represents decimal
    }
}

int binaryTo10(int binary)
{
    int dec=0;
    for (int i=0; binary>0; i++)
    {
        if (binary%10==1)
        {
            dec+=(1<<i);
        }
        binary/=10;
    }
    return dec;
}

void printToFile(string dec,int padding,string codes[])
{
    //make header string
    string codesForHeader;
    for (int i=0; i<128; i++) //adds code for each character
    {
        if (codes[i]!="") //if codes[i] is null then that character isn't in file. don't add code of null to header
        {
            codesForHeader=codesForHeader+'3'+(char)i+codes[i]; //3 is flag that next character is a character in file and characters after that until next \n is it's code
        }
    }
    stringstream intToString;
    intToString << padding;
    string pad;
    intToString >> pad;
    string header=pad+codesForHeader+'3'+'3'+'3';//3 3s flag that header is done
    string printMe=header+dec;
    //open out file and print
    ofstream outFile("Compressed War and Peace.comp");
    char *buffer=new char[(int)printMe.length()];
    for (int i=0; i<(int)printMe.length();i++)
    {
        buffer[i]=printMe[i];
    }
    outFile.write(buffer,(int)printMe.length());
    outFile.close();
}

string parseCompressedFile(string codes[],string toBeDecompressed)
{
    //get padding from first char in toBeCompressed
    string pad;
    pad+=(char)toBeDecompressed[0];
    stringstream padStringToInt;
    padStringToInt << pad;
    int padding;
    padStringToInt >> padding;
    string compressedText;
    for (int i=1; i<(int)toBeDecompressed.length(); i++) //scan rest of string (doesn't include padding chars)
    {
        if (toBeDecompressed[i]=='3') //chars until next 3 are character and its code
        {
            int j=i+1;
            if (toBeDecompressed[j]=='3' and toBeDecompressed[j+1]=='3') //end of header
            {
                for (int k=j+2; k<(int)toBeDecompressed.length(); k++) //go through rest of string, this is the text
                {
                    compressedText+=toBeDecompressed[k];
                }
                break; //get back out of for loop immediately, done getting everything
            }
            char character=toBeDecompressed[j]; //next char is the character getting code for
            j++; //move along in header
            while (toBeDecompressed[j]!='3') //go until hit flag for next character
            {
                codes[(int)character]+=toBeDecompressed[j];
                j++; //move along in header
            }
            i=j-1; //jump forward in for loop to end of code. -1 because loop will increase by 1 right after, and that is the position of the 3
        }
    }
    //now have codes for each character in original file and the compressed text
    //convert compressed text to binary and take off padding
    string binaryText=asciiToBinary(compressedText);
    binaryText.erase(binaryText.length()-padding,(int)binaryText.length()); //removes padding
    return binaryText;
}

string asciiToBinary(string compressedText)
{
    string binaryText;
    for (int i=0; i<(int)compressedText.length(); i++) //get each ascii char
    {
        int ascii=(int)compressedText[i];
        int bin=decimalToBin(ascii);
        stringstream intBinToString;
        intBinToString << bin;
        string tempBin;
        intBinToString >> tempBin;
        //add 0s on to front of binary string to make it a full 8 bits-->binary number doesnt change but how you follow in tree does
        string oneBin;
        while ((int)tempBin.length()+(int)oneBin.length()<8) //go until oneBin has enough 0s to make the 2 lengths add to 8
        {
            oneBin+="0";
        }
        oneBin+=tempBin; //adds rest of binary nuber
        binaryText+=oneBin;
    }
    return binaryText;
}

int decimalToBin(int ascii)
{
    if (ascii<0)
    {
        ascii+=256; //overflowed to be a negative value, we worked with equivalent decimal values that were postive when compressing
    }
    int bin=0;
    int rem;
    int i=1;
    do
    {
        rem=ascii%2;
        bin=bin+(i*rem);
        ascii=ascii/2;
        i=i*10;
    }while(ascii>0);
    return bin;
}

void makeTree(Node **root,string code,char character)
{
    Node *current=*root; //keep track of start of tree and where we are in tree
    for (int i=0; i<(int)code.length(); i++) //scan entire code and add every branch for every char
    {
        if (code[i]=='0')
        {
            if (current->left==NULL)
            {
                Node *newNode=new Node;
                initNode(newNode,false,0,'a'); //0 is placeholder, won't access, it's not a leaf until set it after for loop (when we know whether leaf or not)
                newNode->parent=current;
                current->left=newNode;
            }
            current=current->left;
        }
        else //=='1'
        {
            if (current->right==NULL)
            {
                Node *newNode=new Node;
                initNode(newNode,false,0,'a'); //0 is placeholder, won't access, it's not a leaf until set it after for loop (when we know whether leaf or not)
                newNode->parent=current;
                current->right=newNode;
            }
            current=current->right;
        }
    }
    //at end of code, therefore at a leaf/character
    current->leaf=true;
    current->character=character;
    while (current->parent!=NULL) //gets current back to root
    {
        current=current->parent;
    }
    *root=current;
}

string decompress(string compressedText,Node *root)
{
    string text;
    Node *current=root;
    for (int i=0; i<(int)compressedText.length(); i++)
    {
        if (compressedText[i]=='0')
        {
            current=current->left;
        }
        else //=='1'
        {
            current=current->right;
        }
        if (current->leaf) //at character value, put in string and go back to root
        {
            text+=current->character;
            current=root;
        }
    }
    return text;
}








