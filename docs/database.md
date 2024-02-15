# Database

## Structure of CSV file

- A compatible CSV file needs to have the following columns: `IDX,GR,MAB,MORF,CAT`.
- **IDX** is a number that codifies the position of a word in the text:
    - two first numbers tell which book is. For example: `40` is Matthew.
    - three digits tell the chapter.
    - three digits tell the versicle.
    - last three digits tell the word (in the versicle). In each versicle starts from 1 again.
- **GR** word in greek
- **MAB** code in the MAB Biblical Greek dictionary identificator.
- **MORF** gramatical description of the greek word.
- **CAT** is the catalan translation or equivalence.
