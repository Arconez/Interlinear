#ifndef _FILE_H_
#define _FILE_H_

#include <iostream>
#include <iomanip>

using namespace std;

const string CSV_FILENAME = "db/EQUIV.csv";
const string DB_FILENAME = "db/EQUIV.db";
const string RAW_FOLDER = "raw/";
const string TEX_FOLDER = "tex/";

class File {
public:
    string name;
    string raw;
    string tex;
    int book;
    int chapter;

    File(int book, int chapter) : book(book), chapter(chapter) {
        makeNumber();
        makeRawPath();
        makeTexPath();
    };

    void makeNumber() {
        stringstream ss;
        ss << setw(2) << setfill('0') << right << dec << book;
        ss << "-";
        ss << setw(2) << setfill('0') << right << dec << chapter;
        name = ss.str();
    }

    void makeRawPath() {
        stringstream ss;
        ss << RAW_FOLDER << name << ".md";
        raw = ss.str();
    }

    void makeTexPath() {
        stringstream ss;
        ss << TEX_FOLDER << name << ".tex";
        raw = ss.str();
    }
};

#endif