#include "book.h"

Book::Book() {
    db = new Database(CSV_FILENAME, DB_FILENAME);
}

void Book::update_database() {
    db->importFromCSV();
}

void Book::render() {
    system("xelatex tex/index.tex");
}

bool Book::render_versicle(string line) {
    int _word = 1;
    
    if(line.empty()) {
        stringstream msg;
        msg << "ADVERTÈNCIA A " << BIBLE_BOOKS_SHORT[_book] << " (" << _chapter << ":" << _versicle << ")" << endl;
        msg << " ==> Línia buida." << endl;
        msg << "[Omet i continua]" << endl;
        messages.push(msg.str());
        return true;
    }

    // paragraph title (versicle)
    _output << "\\paragraph{(" << BIBLE_BOOKS_SHORT[_book] << " ";
    _output << _chapter << ":" << _versicle << ")}" << endl;

    stringstream ss(line);
    string word;
    while(getline(ss, word, ' ')) {

        // Word processing
        DataRow dr = DataRow(_book, _chapter, _versicle, _word, word);
        
        db->searchByPosition(dr);
        cout << dr.show() << endl;
        bool word_found = (dr.catalan == "");

        if(word_found) {
            bool same_word = (dr.greek == word);
            if(same_word) {
                // Replace word
                _output << "\\trad{" << word << "}{" << dr.catalan << "} ";
            } else {
                // Error
                stringstream msg;
                msg << "ERROR A " << BIBLE_BOOKS_SHORT[_book] << " (" << _chapter << ":" << _versicle << ")" << endl;
                msg << " ==> Error de concordança entre paraules: al text '" << word;
                msg << "' i a la base de dades '" << dr.greek << "'." << endl;
                msg << "[Aborta processament capítol]" << endl;
                messages.push(msg.str());
                return false;
            }
        } else {
            // Word not found (try again)
            vector<DataRow> results;
            try {
                results = db->searchByWord(dr);
            } catch (exception e) {
                // Error
                stringstream msg;
                msg << "ERROR A " << BIBLE_BOOKS_SHORT[_book] << " (" << _chapter << ":" << _versicle << ")" << endl;
                msg << " ==> La paraula al text '" << word;
                msg << "' no es troba a la base de dades." << endl;
                msg << "[Aborta processament capítol]" << endl;
                messages.push(msg.str());
                return false;
            }

            stringstream msg;
            string default_word = results[0].catalan;
            msg << "ADVERTÈNCIA A " << BIBLE_BOOKS_SHORT[_book] << " (" << _chapter << ":" << _versicle << ")" << endl;
            msg << " ==> La paraula al text '" << word << "' genera diversos resultats." << endl;
            msg << "[Per defecte el primer resultat '" << default_word << "']" << endl;
            messages.push(msg.str());

            // By default, the first one
            _output << "\\trad{" << word << "}{ " << default_word << "} ";
        }
        ++_word;
    }

    return true;
}

bool Book::render_chapter() {
    // Opening file streams
    File f(_book, _chapter);
    ifstream input(f.raw);
    _output.open(f.tex);

    // Stopping condition (inside the loop)
    if(not input.is_open()) {
        cout << "Error: file " << f.raw << " does not exist. Continue next book." << endl;
        return false;
    }
    if(not _output.is_open()) {
		cout << "Error: file " << f.tex << " cannot be opened" << endl;
		return false;
	}

    cout << "Processing: " << BIBLE_BOOKS_SHORT[_book] << " : " << _chapter << endl;

    _versicle = 1;
    string line = "";
    getline(input, line);

    // First line is the title if starts by "#"
    if (line.rfind("# ", 0) == 0) {
        _output << "\\section{" << _chapter << "}" << endl;
	} else {
        input.clear();
        input.seekg(0, ios::beg);
    }

    // Reading before the title
    while(getline(input,line)) {
        // Rendering each versicle (versicle = line of the file)
        bool correct = render_versicle(line);
        if(not correct) break;
		++_versicle;
    }

    _output.close();
    chapters.push_back(f.name);
    return true;
}

void Book::generate_index() {
    // creating preamble and file index
    /*stringstream ss;
    ss << TEX_FOLDER << "index.tex";
    ofstream output;
    output.open(ss.str());*/

    string index = string(TEX_FOLDER) + string("index.tex");
    ofstream output(index);

    // Inserting file settings
    output << "\\input{preamble}" << endl;
    
    // Inserting references for each file
    output << "\\begin{document}" << endl;
    output << "\\maketitle" << endl;
    output << "\\tableofcontents" << endl;
    output << "\\mainmatter" << endl;

    for(int i = 0; i < chapters.size(); ++i) {
        // \include only for chapters/sections;
        // if not, \input
        output << "\\input{" << TEX_FOLDER << chapters[i] << "}" << endl;
    }

    output << "\\end{document}" << endl;
    output.close();
}

void Book::dump_messages() {
    ofstream dump("processing_book.log");
    while(not messages.empty()) {
        dump << messages.front() << endl;
        messages.pop();
    }
    dump.close();
}

void Book::generate() {
    _book = MATT;
    while(_book >= MATT and _book <= REV) {
        _chapter = 1;
        bool rendered = true;

        // Bible book title (a Bible book is a Latex chapter)
        _output << "\\chapter{" << BIBLE_BOOKS[_book] << "}" << endl;

        // If render_chapter returns false, breaks the loop.
        while(rendered) {
            rendered = render_chapter();
            ++_chapter;
        }
        ++_book;
    }

    generate_index();
    dump_messages();
}

