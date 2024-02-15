#ifndef _DATAROW_H_
#define _DATAROW_H_

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

enum DataRowIdx {
    BOOK = 0, CHAPTER, VERSICLE, WORD,
    GREEK, MAB, MORF, CATALAN
};

class DataRow{
//private:
public:
    int book;
    int chapter;
    int versicle;
    int word;
    string greek;
    string catalan;
    string mab;
    string morf;

    void parseBookmark(string idx);

//public:
    DataRow() {
        
    };

    // Use this constructor when parsing a raw greek file
    DataRow(int book, int chapter, int versicle, int word, string greek) {
        this->book = book;
        this->chapter = chapter;
        this->versicle = versicle;
        this->word = word;
        this->greek = greek;
    }

    // Use this constructor when parsing CSV database
    DataRow(string line) { this->parseLine(line); }
    void parseLine(string line);

    // Accessor for insert queries
    vector<string> values();

    // Accessors used when building the preprocessed tex file
    string get_catalan() { return this->catalan; }
    string get_mab() { return this->mab; }
    string get_morf() { return this->morf; }

    // Display for debug
    string show();
};

#endif