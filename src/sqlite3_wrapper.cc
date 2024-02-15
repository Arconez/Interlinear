
#include "sqlite3_wrapper.h"

SQLiteWrapper::SQLiteWrapper() : db(nullptr) {}

SQLiteWrapper::~SQLiteWrapper() {
    if (db)
        sqlite3_close(db);
}

bool SQLiteWrapper::open(const char *dbName) {
    int rc = sqlite3_open(dbName, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

bool SQLiteWrapper::executeQuery(const char *sql) {
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool SQLiteWrapper::executeQueryWithCallback(const char *sql, int (*callback)(void *, int, char **, char **), void *data) {
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// Example callback function for select queries
/*int callback(void *data, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}*/
