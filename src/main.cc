
#include <iostream>
#include <iomanip>

#include "database.h"
#include "sqlite3_wrapper.h"

using namespace std;

const string CSV_FILENAME = "db/EQUIV.csv";
const string DB_FILENAME = "db/EQUIV.db";
const string RAW_FOLDER = "raw/";
const string TEX_FOLDER = "tex/";


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


string scan_files(string folder) {
    stringstream ss;
    ss << "ls " << folder;
    string x = ss.str();
    char* cmd = new char[x.size()+1];
    strcpy(cmd, x.c_str());

    // Executing bash command
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

// TODO: IT SHOULD GET RAW_FILENAME, TEX_FILENAME AND BOOK,CHAPTER NUMBERS
void rendering_file(Database &db, string filename) {
    stringstream ss;
    ss << RAW_FOLDER << filename << ".md";
    ifstream input_file(ss.str());

    stringstream st;
    st << TEX_FOLDER << filename << ".tex";
    ofstream output;
	output.open(st.str());

    if(not output.is_open()) {
		cout << "Error: file " << st.str() << " cannot be opened" << endl;
		return;
	}

    int pos_filename = filename.find("-");
    string num_book = filename.substr(0,pos_filename);
    string num_chapter = filename.erase(0,pos_filename+1);

    int book = stoi(num_book);
    int chapter = stoi(num_chapter);
    int versicle = 1;

    cout << "Processing BOOK" << book << " : " << chapter << endl;

    // chapter title (bible book)
    if(chapter == 1) {
        output << "\\chapter{" << BIBLE_BOOKS[book] << "}" << endl;
    }

    string line;
    while(getline(input_file,line)) {

        // checking title
		if (line.rfind("# ", 0) == 0) {
			/*string title = line.erase(0,2);
			output << "\\section{" << title << "}" << endl;*/

            // section title (chapter)
            output << "\\section{" << chapter << "}" << endl;

			continue;
		}

        int word = 1;
		if(line == "") continue;
		string str_word = "";
		bool continue_loop = true;

        // paragraph title (versicle)
        output << "\\paragraph{(" << BIBLE_BOOKS_SHORT[book] << " ";
        output << chapter << ":" << versicle << ")}" << endl;
		
		while(continue_loop) {
			int pos = line.find(" ");
			if(pos == string::npos) {
				continue_loop = false;
				str_word = line;
			} else {
				str_word = line.substr(0,pos);
				line = line.erase(0, pos+1);
			}
			
			//cout << "> " << str_word << endl;

            DataRow dr = DataRow();
            dr.book = book;
            dr.chapter = chapter;
            dr.versicle = versicle;
            dr.word = word;
            if(str_word != "") {
                dr.greek = str_word;
            } else {
                // End of the line
                continue;
            }
			
            db.searchByPosition(dr);
            cout << dr.show() << endl;
			
			if(dr.catalan == "") {
                // Word not found (try again)
                vector<DataRow> results;
                try {
                    results = db.searchByWord(dr);
                } catch (exception e) {
                    // no results
                    cout << "Results not found" << endl;
                    continue;
                }

				output << "\\trad{" << str_word << "}{";

                // By default, the first one
                string str_cat;
                cout << "Num results" << results.size() << endl;
                if(results.size() > 0) {
                    str_cat = results[0].catalan;
                    cout << str_cat << endl;
                } else {
                    str_cat = "";
                }

                output << str_cat << "} ";
			} else {
				output << "\\trad{" << str_word << "}{";
                output << dr.catalan << "} ";
			}
			++word;
		}		
		++versicle;

    }

    output.close();
}

int main(int argc, char** argv) {
    string root_directory = "";
    string option = "";
    Database db = Database(CSV_FILENAME, DB_FILENAME);

    bool recreate_database = false;
    bool parse_files = false;
    bool remake_tex = false;

    // First parameter skipped
    for(int i = 1; i < argc; ++i) {
        if(strcmp(argv[i],"render") == 0) {
            parse_files = true;
            remake_tex = true;
        } else if (strcmp(argv[i],"rebuild") == 0) {
            recreate_database = true;
        } else if (strcmp(argv[i],"export") == 0) {
            remake_tex = true;
        } else if (strcmp(argv[i],"test") == 0) {
            // test
            DataRow d;
            d.greek = "φυλακη";
            db.searchByWord(d);

            return 0;
        }
    }

    // Doing tasks commanded
    if(recreate_database) {
        db.importFromCSV();
    }

    if(parse_files) {
        string files = scan_files(RAW_FOLDER);

        vector<string> files_to_append;

        int book_count = MATT;
        while(book_count >= MATT and book_count <= REV) {
            int chapter_count = 1;
            while(true) {
                stringstream ss;
                ss << setw(2) << setfill('0') << right << dec << book_count;
                ss << "-";
                ss << setw(2) << setfill('0') << right << dec << chapter_count;
                string file_num = ss.str();

                ss.str(string());
                ss << RAW_FOLDER << file_num << ".md";
                string file_raw = ss.str();

                ss.str(string());
                ss << TEX_FOLDER << file_num;
                string file_tex = ss.str();

                ifstream chapter(file_raw);
                if(chapter.is_open()) {
                    chapter.close();
                    // existe
                    rendering_file(db, file_num);
                    files_to_append.push_back(file_tex);
                } else {
                    // no more, following chapter
                    break;
                }
                ++chapter_count;
            }
            ++book_count;
        }

        // Creating index file
        // creating preamble and file index
        stringstream ss;
        ss << TEX_FOLDER << "index.tex";
        ofstream output;
	    output.open(ss.str());

        output << "\\documentclass{book}" << endl;
        output << "\\usepackage{fontspec}" << endl;
        output << "\\usepackage[catalan]{babel}" << endl;
        output << "\\setmainfont{Liberation Serif}" << endl;
        output << "\\setsansfont{Liberation Sans}" << endl;
        output << "\\newfontfamily{\\greekfont}{Liberation Serif}" << endl;
        output << "\\newfontfamily{\\greekfontsf}{Liberation Sans}" << endl;
        
        output << "\\setcounter{tocdepth}{1}" << endl;

        output << endl;
        /*output << "\\newcommand\\trad[2]%" << endl;
        output << "{\\begin{tabular}[b]{@{}c@{}}" << endl;
        output << "\\footnotesize #2\\\\" << endl;
        output << "#1" << endl;
        output << "\\end{tabular}%" << endl;
        output << "}" << endl;*/
        output << "\\newcommand\\trad[2]%" << endl;
        output << "{\\begin{tabular}[b]{@{}c@{}}" << endl;
        output << "#1\\\\" << endl;
        output << "\\tiny #2" << endl;
        output << "\\end{tabular}%" << endl;
        output << "}" << endl;
        
        // Main properties of document
        output << "\\title{Nou Testament Interlinial}" << endl;
        output << "\\date{\\today}" << endl;
        output << "\\author{Edició de Josep Manel Suari}" << endl;

        // Inserting references for each file
        output << "\\begin{document}" << endl;
        output << "\\maketitle" << endl;
        output << "\\tableofcontents" << endl;
        output << "\\mainmatter" << endl;
        //output << "\\chapter{Dedication}" << endl;
        //TODO
        //output << "\\chapter{Copyright}" << endl;
        //TODO
        //output << "\\chapter{Acknowledgements}" << endl;
        // TODO

        //....
        for(int i = 0; i < files_to_append.size(); ++i) {
            // \include only for chapters/sections; if not, \input
            output << "\\input{" << files_to_append[i] << "}" << endl;
        }
        //....
        output << "\\end{document}" << endl;
	    output.close();
    }

    if(remake_tex) {
        // Render index file
        system("xelatex tex/index.tex");
    }

    return 0;
}
