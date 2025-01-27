
# Documentation for program Bible

This program, codename _Bible_, serves to create a polished interlinear biblical text.

Nowadays it's hardcoded to perform a interlinear text from Greek to Catalan. (New Testament only)

**Last revision of code and documentation: 11 / 02 / 2024**

## How does it work

The program is composed by these components:
1) The **program itself**, composed by the [source code](source%20code.md#source-code), in the `/src` folder.
2) The **raw files** at `/raw` folder. It contains the greek texts of New Testament books. It has to be codified in a certain way explained [below](raw%20files.md#raw-files).
3) The **preprocessed files** at `/tex` folder. There, Bible dumps the preprocessed TeX files ready to render as a PDF.
4) The **database** that is composed by two files. You can bring your data as a CSV file (that can be created in Excel or another program). Bible generates its own database in SQLite3. It has to follow careful structure to be compatible with Bible.
    - CSV file "EQUIV.csv"
    - SQLite3 database "EQUIV.db"
5) The **output** generated by XeLaTeX.

## Commands
The program has to be called passing it parameters that specify what can you do. Order doesn't matter.

- `rebuild`: Recreates the database from the CSV file.
- `render`: Recreates all TeX files and also calls XeLaTeX to render final PDF (also includes `export`).
- `export`: From the given TeX files, creates a unified TeX file and calls XeLaTeX.

Example: `./bible render`

## Things TO DO
1) improve rendering tasks: it has to be flexible to identify where are raw files, and process them acordingly
2) improve database access, when a word has been seen before and, therefore, has no more repetitions in the database.
3) improve format management
