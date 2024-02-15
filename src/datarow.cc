
#include "datarow.h"

// Converts and IDX description code to a set
// of internal codes of Bookmark
void DataRow::parseBookmark(string idx) {
    this->book = stoi(idx.substr(0,2));
    this->chapter = stoi(idx.substr(2,3));
    this->versicle = stoi(idx.substr(5,3));
    this->word = stoi(idx.substr(8,3));
}

// Parses a line from CSV database to
// dump data to SQLite database
void DataRow::parseLine(string line) {
    int pos = 0;

    // First item (IDX)
    pos = line.find(",");
    if(pos == string::npos) {
        throw new exception();
    }
    string idx = line.substr(0,pos);
    line = line.erase(0, pos+1);
    this->parseBookmark(idx);

    // Second to fourth item
    for(int i = 1; i < 4; ++i) {
        pos = line.find(",");
        if(pos == string::npos) {
            throw new exception();
        }
        string item = line.substr(0,pos);
        line = line.erase(0,pos+1);

        if(i == 1) {
            this->greek = item;
        } else if(i == 2) {
            this->mab = item;
        } else if(i == 3) {
            this->morf = item;
        }
    }

    // Fifth item (last)
    this->catalan = line;
}

// Returns a vector with the values of
// the class ordered to be used in an
// insertion operation
vector<string> DataRow::values() {
    vector<string> obj = { 
        to_string(this->book),
        to_string(this->chapter),
        to_string(this->versicle),
        to_string(this->word),
        this->greek,
        this->mab,
        this->morf,
        this->catalan
    };

    return obj;
}

// Creates a string that shows values for debug
string DataRow::show() {
    stringstream ss;
    ss << "[" << to_string(book) << ",";
    ss << to_string(chapter) << ",";
    ss << to_string(versicle) << ",";
    ss << to_string(word) << ",";
    ss << greek << "," << mab << ",";
    ss << morf << "," << catalan << "]";
    return ss.str();
}