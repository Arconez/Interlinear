#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <sqlite3.h>
#include <cstring>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "datarow.h"
#include "sqlite3_wrapper.h"

using namespace std;

struct Table {
    string name;
    vector<string> cols;
};

class Database {
private:
    // Internal variables
    string csv_file;
    string db_file;
    SQLiteWrapper db;

    // Table definitions
    struct Table EQUIV = {"EQUIV", {
        "BOOK INT NOT NULL",
        "CHAPTER INT NOT NULL",
        "VERSICLE INT NOT NULL",
        "WORD INT NOT NULL",
        "GREEK TEXT NOT NULL",
        "MAB TEXT NOT NULL",
        "MORF TEXT NOT NULL",
        "CATALAN TEXT NOT NULL"
    }};

    // SQL Builders
    const char* SQL_createTable(Table t);
    const char* SQL_insert(string name, vector<string> cols, vector<string> values);
    const char* SQL_select(string name, string selection, string conditions);

    // Callbacks
    static int select_callback(void *data, int argc, char **argv, char **azColName);
    static int select_results(void *data, int argc, char **argv, char **azColName);

    // Internal filesystem operations
    static int countFileLines(string filename);

public:
    Database(string csv_file, string db_file);
    void importFromCSV();
    void insert(string name, vector<string> cols, vector<string> values);
    void insert(DataRow data);
    bool searchByPosition(DataRow &data);
    vector<DataRow> searchByWord(DataRow &data);
};

#endif