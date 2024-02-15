
#include <iostream>
#include <iomanip>

#include "database.h"
#include "sqlite3_wrapper.h"
#include "bible.h"

using namespace std;

const string CSV_FILENAME = "db/EQUIV.csv";
const string DB_FILENAME = "db/EQUIV.db";
const string RAW_FOLDER = "raw/";
const string TEX_FOLDER = "tex/";

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

// Render a specified Bible chapter.
// db is a Database connection to check word data
// rendered_chapters is a list of the rendered files that
// is filled in the function to later know which files have
// to be merged.
// book and chapter are the number of book and chapter
// Returns true if the raw file existed and could be rendered
// If the file doesn't exist, returns false.
bool render_chapter(Database &db, vector<string> &rendered_chapters, int book, int chapter) {
    // Creating filenames
    stringstream ss;
    ss << setw(2) << setfill('0') << right << dec << book;
    ss << "-";
    ss << setw(2) << setfill('0') << right << dec << chapter;
    string file_num = ss.str();

    ss.str(string());
    ss << RAW_FOLDER << file_num << ".md";
    string file_raw = ss.str();

    ss.str(string());
    ss << TEX_FOLDER << file_num;
    string tex_chapter = ss.str();
    ss << ".tex";
    string file_tex = ss.str();

    // Opening file streams
    ifstream input_file(file_raw);
    ofstream output;
	output.open(file_tex);

    // Stopping condition of the render loop
    // If the file does not exist, return false.
    if(not input_file.is_open()) {
        return false;
    }

    if(not output.is_open()) {
		cout << "Error: file " << file_tex << " cannot be opened" << endl;
		return false;
	}

    // Counters
    // int book, int chapter
    int versicle = 1;

    cout << "Processing: " << BIBLE_BOOKS_SHORT[book] << " : " << chapter << endl;

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
            // Tokenizer
			int pos = line.find(" ");
			if(pos == string::npos) {
				continue_loop = false;
				str_word = line;
			} else {
				str_word = line.substr(0,pos);
				line = line.erase(0, pos+1);
			}
			
            if(str_word == "") {
                // End of the line
                continue;
            }

            // Word processing
            DataRow dr = DataRow(book, chapter, versicle, word, str_word);
			
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
    rendered_chapters.push_back(tex_chapter);
    return true;
}

void test(Database &db) {
    DataRow d;
    d.greek = "φυλακη";
    db.searchByWord(d);
}

int main(int argc, char** argv) {
    Database db = Database(CSV_FILENAME, DB_FILENAME);

    // Flags
    bool recreate_database = false;
    bool parse_files = false;
    bool remake_tex = false;

    // First parameter skipped
    for(int i = 1; i < argc; ++i) {
        string param = argv[i];

        if(param == "render") {
            parse_files = true;
            remake_tex = true;
        } else if (param == "rebuild") {
            recreate_database = true;
        } else if (param == "export") {
            remake_tex = true;
        } else if (param == "test") {
            test(db);
        }
    }

    // Doing tasks commanded
    if(recreate_database) {
        db.importFromCSV();
    }

    if(parse_files) {
        //string files = scan_files(RAW_FOLDER);
        vector<string> chapters;

        int book = MATT;
        while(book >= MATT and book <= REV) {
            int chapter = 1;
            bool finished_chapter = false;

            // Breaking condition when render_chapter returns false.
            // So, not false is true, when "finished chapter".
            while(not finished_chapter) {
                finished_chapter = not render_chapter(db, chapters, book, chapter);
                ++chapter;
            }
            ++book;
        }

        // Creating index file
        // creating preamble and file index
        stringstream ss;
        ss << TEX_FOLDER << "index.tex";
        ofstream output;
	    output.open(ss.str());

        // Inserting file settings
        output << "\\input{preamble}" << endl;
        
        // Inserting references for each file
        output << "\\begin{document}" << endl;
        output << "\\maketitle" << endl;
        output << "\\tableofcontents" << endl;
        output << "\\mainmatter" << endl;

        for(int i = 0; i < chapters.size(); ++i) {
            // \include only for chapters/sections; if not, \input
            output << "\\input{" << chapters[i] << "}" << endl;
        }

        output << "\\end{document}" << endl;
	    output.close();
    }

    if(remake_tex) {
        // Render index file
        system("xelatex tex/index.tex");
    }

    return 0;
}
