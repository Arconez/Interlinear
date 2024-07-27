
#include "database.h"

Database::Database(string csv_file, string db_file) {
    this->csv_file = csv_file;
    this->db_file = db_file;
    const char* db_f = db_file.c_str();

    if (this->db.open(db_f)) {
        const char* sql = SQL_createTable(EQUIV);
        //cout << "Text: " << sql << endl;
        db.executeQuery(sql);
        /*db.executeQuery("CREATE TABLE IF NOT EXISTS Users(Id INTEGER PRIMARY KEY, Name TEXT, Age INTEGER)");
        db.executeQuery("INSERT INTO Users (Name, Age) VALUES ('John Doe', 30)");
        db.executeQuery("INSERT INTO Users (Name, Age) VALUES ('Jane Doe', 25)");

        // Select query with callback
        db.executeQueryWithCallback("SELECT * FROM Users", select_callback, nullptr);
        */
    }
}

// Example callback function for select queries
int Database::select_callback(void *data, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

struct ResultRow {
    string result;
};

struct ResultRows {
    //DataRow query;
    vector<DataRow> results;
};

// Private functions

// Returns the first word of a string
string first_word(string str) {
    int pos = str.find(" ");
    if (pos == string::npos) return str;
    else return str.substr(0, pos);
}

// Check if a string is a CSV header for EQUIV.csv
bool is_CSV_header(string line) {
    return line == "IDX,GR,MAB,MORF,CAT";
}

// Simple callback for queries that doesn't expect return
int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        cout << azColName[i] << " = ";
        cout <<  (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << endl;
    return 0;
}

// Simple callback to get only one result
int select_one_result(void *data, int argc, char **argv, char **azColName) {
    /*cout << "Args: " << argc << endl;
    if(argc != 1) {
        cout << "results not found!" << endl;
    }*/
    
    //cout << "---- " << argv[0] << endl;
    DataRow *res = (DataRow*)data;
    if(argv == nullptr) {
        cout << "results not found" << endl;
        res->catalan = "";
    } else {
        res->catalan = string(argv[0]);
    }

    return 0;
}

// Callback to get several results (return a vector of DataRow)
int Database::select_results(void *data, int argc, char **argv, char **azColName) {
    cout << "Args: " << argc << endl;
    /*if(argc == 0) {
        cout << "results not found!" << endl;
        return 0;
    }*/

    /*for(int i = 0; i<argc; i++) {
        cout << i << " " << azColName[i] << " = ";
        cout <<  (argv[i] ? argv[i] : "NULL") << endl;
    }*/

    if(data != nullptr) {
        ResultRows *results = (ResultRows*)data;

        DataRow result;
        result.book = atoi(argv[DataRowIdx::BOOK]);
        result.chapter = atoi(argv[DataRowIdx::CHAPTER]);
        result.versicle = atoi(argv[DataRowIdx::VERSICLE]);
        result.word = atoi(argv[DataRowIdx::WORD]);
        result.greek = argv[DataRowIdx::GREEK];
        result.mab = argv[DataRowIdx::MAB];
        result.morf = argv[DataRowIdx::MORF];
        result.catalan = argv[DataRowIdx::CATALAN];

        results->results.push_back(result);
    }

    return 0;
}
    
// Given a name and column description, creates an SQL
// query in const char* to send to SQLite database
const char* Database::SQL_createTable(Table t) {
    unsigned int num_cols = t.cols.size();
    if(num_cols == 0) {
        std::cout << "createTable ERROR: empty cols list" << endl;
        return "";
    }

    stringstream ss;
    ss << "CREATE TABLE IF NOT EXISTS " << t.name << " (";
    ss << t.cols[0];
    for(int i = 1; i < num_cols; ++i) {
        ss << ", " << t.cols[i];
    }
    ss << ")";

    // Conversion to c_str
    string x = ss.str();
    char* c_str = new char[x.size()+1];
    strcpy(c_str, x.c_str());
    return c_str;
}

// Given a table name, column names and values,
// creates an SQL query in const char* to send to
// SQLite database
const char* Database::SQL_insert(string name, vector<string> cols,
    vector<string> values) {
    unsigned int num_cols = cols.size();
    unsigned int num_values = values.size();
    //unsigned int num_rows = num_values / num_cols;

    if(num_values != num_cols) {
        cout << "insert ERROR: there are leading values" << endl;
        return "";
    }

    stringstream ss;
    ss << "INSERT INTO " << name << " (";
    // Cols; only first word, until first space
    ss << first_word(cols[0]);
    for(int i = 1; i < num_cols; ++i) {
        ss << ", " << first_word(cols[i]);
    }
    ss << ") VALUES (\"" << values[0] << "\"";
    for(int i = 1; i < num_values; ++i) {
        ss << ", \"" << values[i] << "\"";
    }
    ss << ")";
    
    // Conversion to c_str
    string x = ss.str();
    char* c_str = new char[x.size()+1];
    strcpy(c_str, x.c_str());
    return c_str;
}

// Given a table name, selected columns and
// conditions (where, limit, etc. No parsing
// is done here), ccreates an SQL query in
// const char* to send to SQLite database
const char* Database::SQL_select(string name, string selection="*", string conditions="") {
    stringstream ss;
    ss << "SELECT " << selection << " FROM " << name;
    if(conditions != "") {
        ss << " WHERE " << conditions;
    }

    // Conversion to c_str
    string x = ss.str();
    char* c_str = new char[x.size()+1];
    strcpy(c_str, x.c_str());
    return c_str;
}

// Count CSV lines in order to show dumping progress
int Database::countFileLines(string filename) {
    stringstream ss;
    ss << "wc -l " << filename;
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
    
    // The value is the first word (before first space)
    int pos = result.find(" ");
    string number = result.substr(0,pos);
    return stoi(number);
}

void Database::insert(string name, vector<string> cols, vector<string> values) {
    const char *sql = SQL_insert(name, cols, values);
    db.executeQuery(sql);
}

void Database::insert(DataRow data) {
    this->insert(EQUIV.name, EQUIV.cols, data.values());
}

void Database::importFromCSV() {
    // Table has been created
    // Delete preventively raws in EQUIV
    db.executeQuery("DELETE FROM EQUIV");

    // Table empty, we can fill it again
    ifstream csv(csv_file);
    string line;
    int count_line = 0;

    if(not csv.is_open()) {
        cout << "File not open. error" << endl;
        return;
    }

    int csv_lines = countFileLines(csv_file);

    while(getline(csv, line)){
        if(count_line == 0) {
            if(is_CSV_header(line)) {
                ++count_line;
                continue;
            } else {
                cout << "Error: CSV header missing" << endl;
                return;
            }
        } else {
            // Writing progress line (lines processed and missing)
            cout << "Processing line " << count_line << " of " << csv_lines << endl;
        }

        if(line == "") {
            ++count_line;
            continue;
        }

        try{
            DataRow dr = DataRow(line);
            //cout << dr.show() << endl;
            this->insert(dr);
        }
        catch(exception e) {
            cout << e.what() << endl;
            cout << "Error parsing line" << count_line << endl;
            continue;
        }

        //DEBUG:
        //if(count_line == 50) break;
        ++count_line;
    }
    csv.close();
}

// Used to search a given word in a Datarow structure
// and it is used to fill the Datarow from
// data of the Database.
// Returns true if has returned value; if not, false.
bool Database::searchByPosition(DataRow &data) {
    stringstream ss;
    ss << "BOOK = " << data.book;
    ss << " AND CHAPTER = " << data.chapter;
    ss << " AND VERSICLE = " << data.versicle;
    ss << " AND WORD = " << data.word;
    ss << " AND GREEK = \"" << data.greek << "\" LIMIT 1";
    string conditions = ss.str();

    const char* sql = SQL_select("EQUIV","CATALAN", conditions);
    //cout << sql << endl;
    db.executeQueryWithCallback(sql, select_one_result, &data);

    return (data.catalan != "");
}

vector<DataRow> Database::searchByWord(DataRow &data) {
    stringstream ss;
    ss << "GREEK = \"" << data.greek << "\"";
    string conditions = ss.str();

    const char* sql = SQL_select("EQUIV","*", conditions);
    cout << sql << endl;

    ResultRows res;
    //res.query = data;
    res.results = vector<DataRow>();

    db.executeQueryWithCallback(sql, select_results, &res);

    /*cout << "Results obtained: " << res.results.size() << endl;
    for(int i = 0; i < res.results.size(); ++i) {
        cout << res.results[i].catalan << endl;
    }*/

    return res.results;
}