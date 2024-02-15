#ifndef _BIBLE_H_
#define _BIBLE_H_

#include <iostream>
#include <vector>

using namespace std;

enum BibleBook {
    // Old Testament
    // TODO:

    // New Testament
    MATT = 40, MARK, LUKE, JOHN,
    ACTS, ROM, COR_A, COR_B, GAL,
    EPH, PHIL, COL, THES_A, THES_B,
    TIM_A, TIM_B, TITUS, PHLM, HEB,
    JAS, PET_A, PET_B, JOHN_A, JOHN_B,
    JOHN_C, JUDE, REV
};

// Books in protestant bible
const vector<string> BIBLE_BOOKS = {"",
    // Old Testament
    "Gènesi", "Èxode", "Levític", "Nombres", "Deuteronomi",     //1-5
    "Josuè", "Jutges", "Rut", "Primer llibre de Samuel",        //6-9
    "Segon llibre de Samuel", "Primer llibre dels Reis",        //10-11
    "Segon llibre dels Reis", "Primer llibre de les Cròniques", //12-13
    "Segon llibre de les Cròniques", "Esdres", "Nehemies",      //14-16
    "Ester", "Job", "Salms", "Proverbis", "Eclesiastès",        //17-21
    "Càntic de Salomó", "Isaïes", "Jeremies", "Lamentacions",   //22-25
    "Ezequiel", "Daniel", "Osees", "Joel", "Amós", "Abdies",    //26-31
    "Jonàs", "Miquees", "Nahum", "Habacuc", "Sofonies", "Ageu", //32-37
    "Zacaries", "Malaquies",                                    //38-39

    // New Testament
    "Evangeli segons Mateu", "Evangeli segons Marc",            //40-41
    "Evangeli segons Lluc", "Evangeli segons Joan",             //42-43
    "Fets dels Apòstols", "Carta als Romans",                   //44-45
    "Primera carta als Corintis", "Segona carta als Corintis",  //46-47
    "Carta als Gàlates", "Carta als Efesis",                    //48-49
    "Carta als Filipencs", "Carta als Colossencs",              //50-51
    "Primera carta als Tessalonicencs",                         //52
    "Segona carta als Tessalonicencs",                          //53
    "Primera carta a Timoteu", "Segona carta a Timoteu",        //54-55
    "Carta a Titus", "Carta a Filemó", "Epístola als Hebreus",  //56-58
    "Carta de Jaume",                                           //57
    "Primera carta de Pere", "Segona carta de Pere",            //58-59
    "Primera carta de Joan", "Segona carta de Joan",            //60-61
    "Tercera carta de Joan", "Carta de Judes",                  //62-63
    "Llibre de les Revelacions"                                 //64
};

const vector<string> BIBLE_BOOKS_SHORT = { "",
    // Old Testament
    "Gn", "Ex", "Lv", "Nm", "Dt", "Jos", "Jt", "Rt", "1 Sa",    // 1-9
    "2 Sa", "1 Re", "2 Re", "1 Cr", "2 Cr", "Esd", "Ne", "Est", // 10-17
    "Jb", "Sl", "Pr", "Ecl", "Ct", "Is", "Jr", "Lm", "Ez",      // 18-26
    "Dn", "Os", "Jl", "Am", "Ab", "Jon", "Mi", "Na", "Ha",      // 27-35
    "So", "Ag", "Za", "Ml",                                     // 36-39

    // New Testament
    "Mt", "Mc", "Lc", "Jn", "Ac", "Rm", "1 Co", "2 Co", "Ga",
    "Ef", "Fl", "Col", "1 Te", "2 Te", "1 Tm", "2 Tm", "Tt",
    "Flm", "He", "Jm", "1 Pe", "2 Pe", "1 Jn", "2 Jn", "3 Jn",
    "Jud", "Ap"
};

#endif