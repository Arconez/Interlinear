/**
 * main.cc
 * 
 * INTERLINEAR MAIN FILE
 * ----------------------
 * The goal of this file is to render a Bible Interlinear book.
 * It is not yet a generalized code for any book.
*/

#include <iostream>

#include "book.h"

using namespace std;

void message() {
    cout << "INTERLINEAR TERMINAL VERSION" << endl;
    cout << "\trender : genera PDF desde archivos .md" << endl;
    cout << "\trebuild : crea base de datos Sqlite desde CSV" << endl;
    cout << "\texport : genera PDF desde archivos .tex" << endl;
}

int main(int argc, char** argv) {
    //Database db = Database(CSV_FILENAME, DB_FILENAME);
    Book book = Book();

    // Flags
    bool recreate_database = false;
    bool _parse_files = false;
    bool remake_tex = false;

    // First parameter skipped
    if(argc == 2) {
        string param = argv[1];
        if(param == "render") {
            _parse_files = true;
            remake_tex = true;
        } else if (param == "rebuild") {
            recreate_database = true;
        } else if (param == "export") {
            remake_tex = true;
        } else message();
    } else message();

    // Doing tasks commanded
    if(recreate_database) book.update_database();
    if(_parse_files) book.generate();
    if(remake_tex) book.render();

    return 0;
}
