#ifndef _BOOK_H_
#define _BOOK_H_

#include <iostream>
#include <queue>
#include <vector>
#include <sstream>

#include "database.h"
#include "sqlite3_wrapper.h"
#include "bible.h"
#include "file.h"

using namespace std;

class Book {
private:
    Database *db;
    queue<string> messages;
    vector<string> chapters;
    
    // Internal counters
    int _book;
    int _chapter;
    int _versicle;
    int _word;

    // Shared output for rendering process
    ofstream _output;

    void dump_messages();
    void generate_index();
    bool render_chapter();
    bool render_versicle(string line);

public: 
    Book();
    void update_database();
    void generate();
    void render();
};

#endif