bible:
	g++ -o bible src/main.cc src/sqlite3_wrapper.cc src/datarow.cc src/database.cc -lsqlite3