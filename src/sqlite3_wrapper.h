#ifndef _SQLITE_H_
#define _SQLITE_H_

#include <sqlite3.h>
#include <iostream>

using namespace std;

class SQLiteWrapper {
private:
    sqlite3 *db;

public:
    SQLiteWrapper();
    ~SQLiteWrapper();
    bool open(const char *dbName);
    bool executeQuery(const char *sql);
    bool executeQueryWithCallback(const char *sql, int (*callback)(void *, int, char **, char **), void *data);
};

#endif