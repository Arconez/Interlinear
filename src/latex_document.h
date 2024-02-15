#ifndef _LATEX_DOCUMENT_H_
#define _LATEX_DOCUMENT_H_

#include <iostream>
#include <sstream>

using namespace std;

string trad2(string word, string translation) {
    stringstream ss;
    ss << "\\trad{" << word << "}{";
    ss << translation << "}";
}

#endif