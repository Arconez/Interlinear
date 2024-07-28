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
        msg << "ADVERTÈNCIA [" << _book << "_" << _chapter << "] A " << BIBLE_BOOKS_SHORT[_book] << " (" << _chapter << ":" << _versicle << ")" << endl;
        msg << " ==> Línia buida." << endl;
        msg << "[Omet i continua]" << endl;
        messages.push(msg.str());
        return true;
    }

    // paragraph title (versicle)
    // OLD FORMAT
    /*_output << "\\paragraph{(" << BIBLE_BOOKS_SHORT[_book] << " ";
    _output << _chapter << ":" << _versicle << ")}" << endl;*/

    // SIMPLER NEW FORMAT
    _output << endl << "\\verse ";

    stringstream ss(line);
    string word;
    while(getline(ss, word, ' ')) {

        // Word processing
        DataRow dr = DataRow(_book, _chapter, _versicle, _word, word);
        
        bool word_found = db->searchByPosition(dr);
        cout << dr.show() << endl;

        /* searchByPosition search by position and gives the word 
            that is in the text. If is not the same, yields error. */

        if(word_found) {
            // Replace word
            _output << "\\trad{" << word << "}{" << dr.catalan << "} ";
        } 
        else {
            // Word not found (try again)
            vector<DataRow> results = db->searchByWord(dr);
            
            if(results.empty()) {
                // Error
                stringstream msg;
                msg << "ERROR [" << _book << "_" << _chapter << "] A " << BIBLE_BOOKS_SHORT[_book] << " (" << _chapter << ":" << _versicle << ")" << endl;
                msg << " ==> La paraula " << _word << " al text '" << word;
                msg << "' no es troba a la base de dades." << endl;
                msg << "[Aborta processament capítol]" << endl;
                messages.push(msg.str());
                return false;
            } else {
                stringstream msg;
                string default_word = results[0].catalan;

                if(NOTIFY_WARNINGS) {
                    msg << "ADVERTÈNCIA [" << _book << "_" << _chapter << "] A " << BIBLE_BOOKS_SHORT[_book] << " (" << _chapter << ":" << _versicle << ")" << endl;
                    msg << " ==> La paraula " << _word << " al text '" << word << "' genera diversos resultats." << endl;
                    msg << "[Per defecte el primer resultat '" << default_word << "']" << endl;
                    messages.push(msg.str());
                }

                // By default, the first one
                _output << "\\trad{" << word << "}{ " << default_word << "} ";
            }
        }
        ++_word;
    }

    _output << endl; // End of verse

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
        _output.close();

        stringstream msg;
        msg << "ERROR [" << _book << "_" << _chapter << "] A " << BIBLE_BOOKS_SHORT[_book] << " Cap." << _chapter << endl;
        msg << " ==> No es pot obrir fitxer d'entrada." << endl;
        msg << "[Ignora i continua]" << endl;
        messages.push(msg.str());

        return false;
    }
    if(not _output.is_open()) {
		cout << "Error: file " << f.tex << " cannot be opened" << endl;
        _output.close();

        stringstream msg;
        msg << "ERROR [" << _book << "_" << _chapter << "] A " << BIBLE_BOOKS_SHORT[_book] << " Cap." << _chapter << endl;
        msg << " ==> No es pot obrir fitxer de sortida." << endl;
        msg << "[Ignora i continua]" << endl;
        messages.push(msg.str());

		return false;
	}

    cout << "Processing: " << BIBLE_BOOKS_SHORT[_book] << " : " << _chapter << endl;

    _versicle = 1;
    string line = "";
    getline(input, line);

    // First line is the title if starts by "#"
    if (line.rfind("# ", 0) == 0) {
        // OLD FORMAT
        //_output << "\\section{" << _chapter << "}" << endl;

        // NEW FORMAT
        _output << "\\begin{biblechapter}" << endl;
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

    // Closing chapter (NEW FORMAT)
    _output << "\\end{biblechapter}" << endl;

    _output.close();
    chapters.push_back(f.name);
    return true;
}

void Book::generate_index() {
    string index = string(TEX_FOLDER) + string("index.tex");
    ofstream output(index);

    // Inserting file settings
    output << "\\input{preamble}" << endl;
    
    // Inserting references for each file
    output << "\\begin{document}" << endl;
    output << "\\maketitle" << endl;
    output << "\\tableofcontents" << endl;
    output << "\\mainmatter" << endl;

    // NEW FORMAT SETTINGS
    output << "\\setlength{\\columnseprule}{.5pt}" << endl;

    _book = 0;
    for(int i = 0; i < chapters.size(); ++i) {
        // \include only for chapters/sections;
        // if not, \input

        // Adding book title when start
        int book = stoi(chapters[i].substr(0,2));
        if(book > _book) {
            _book = book;
            // NEW FORMAT 
            output << "\\book{" << BIBLE_BOOKS[_book] << "}" << endl;
        }

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

