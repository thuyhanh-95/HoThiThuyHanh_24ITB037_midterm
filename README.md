# myls - System Programming Midterm

## 1. Student Information

- **Name:** Ho Thi Thuy Hanh
- **Student ID:** 24ITB037

## 2. Project Description

This project implements a simplified version of the NetBSD `ls(1)` command in the C programming language.

The program is named **`myls`** and is developed according to the provided **NetBSD 10.1 `ls(1)` manual**.

The main purpose of `myls` is to list files and directories and display additional file information according to the selected command-line options.

When the operand is a file, the program displays information about that file. When the operand is a directory, the program lists the contents of that directory. If no operand is specified, the program lists the contents of the current directory.

## 3. Supported Options

The program supports the following options:

```text
-A -a -c -d -F -f -h -i -k -l -n -q -R -r -S -s -t -u -w
Option	Description
-A	Display all entries except . and ...
-a	Include hidden entries whose names begin with ..
-c	Use file status change time instead of modification time.
-d	Display the directory itself instead of listing its contents.
-F	Append a symbol indicating the file type.
-f	Disable sorting.
-h	Display file sizes in human-readable format.
-i	Display the inode number.
-k	Display block sizes in kilobytes.
-l	Display information in long format.
-n	Display numeric UID and GID instead of owner and group names.
-q	Replace non-printable characters in file names with ?.
-R	Recursively list subdirectories.
-r	Reverse the sorting order.
-S	Sort entries by file size, largest first.
-s	Display the number of file system blocks used.
-t	Sort entries by modification time, newest first.
-u	Use access time instead of modification time.
-w	Display non-printable characters in raw form.
4. Usage

The general syntax is:

./myls [options] [file ...]
Examples

List the current directory:

./myls

List the contents of the tests directory:

./myls tests

Display hidden files:

./myls -a tests

Display hidden files except . and ..:

./myls -A tests

Display long format:

./myls -l tests

Display human-readable sizes:

./myls -lh tests

Sort by file size:

./myls -S tests

Sort by modification time:

./myls -t tests

Reverse the sorting order:

./myls -r tests

Recursively list subdirectories:

./myls -R tests

Display directories as plain files:

./myls -d tests

Classify file types:

./myls -F tests

Display inode numbers:

./myls -i tests

Display block usage:

./myls -s tests

Display numeric owner and group IDs:

./myls -n tests

Display non-printable characters as ?:

./myls -q tests

Display non-printable characters in raw form:

./myls -w tests
5. Project Structure
myls/
├── .gitignore
├── Makefile
├── README.md
├── include/
│   ├── display.h
│   ├── listing.h
│   ├── options.h
│   └── sorting.h
├── src/
│   ├── display.c
│   ├── listing.c
│   ├── main.c
│   ├── options.c
│   └── sorting.c
└── tests/
    ├── alpha.txt
    ├── beta.log
    ├── notes.md
    ├── tiny.dat
    ├── middle.dat
    ├── huge.dat
    ├── .env_sample
    ├── runme
    ├── shortcut
    ├── pipe_test
    ├── docs/
    │   └── report.txt
    └── nested/
        └── level2/
            └── deep.txt
6. Module Description
main.c

Handles program initialization, command-line arguments and operands.

options.c

Parses command-line options and stores the selected options in the program configuration.

listing.c

Handles directory traversal, file metadata retrieval, hidden-file filtering and recursive directory processing.

sorting.c

Handles sorting entries by name, size and time, as well as reverse ordering.

display.c

Handles output formatting, including long format, file size, inode, owner, group, file classification and symbolic-link targets.

include/

Contains the header files used by the source modules.

tests/

Contains files, directories and special entries used to test the program.

7. Compilation

The project uses Makefile for compilation.

Build the program:

make

Clean compiled files:

make clean

A successful compilation produces:

myls

Compiled binaries and object files are excluded from Git using .gitignore.

8. Testing

The program was tested with the following commands:

Basic listing
./myls tests
Hidden files
./myls -a tests
./myls -A tests
Long format
./myls -l tests
./myls -lh tests
./myls -n tests
Inode and block information
./myls -i tests
./myls -s tests
./myls -sk tests
Sorting
./myls -S tests
./myls -Sr tests
./myls -t tests
./myls -tr tests
./myls -f tests
Recursive and directory modes
./myls -R tests
./myls -d tests
File classification
./myls -F tests
Non-printable characters
./myls -q tests
./myls -w tests
Time selection
./myls -lt tests
./myls -ltu tests
./myls -ltc tests
Option precedence
./myls -ln tests
./myls -nl tests

./myls -Rd tests
./myls -dR tests

./myls -qw tests
./myls -wq tests

For option pairs that override each other, the last specified option determines the final behavior.

BLOCKSIZE
BLOCKSIZE=1024 ./myls -s tests
BLOCKSIZE=4096 ./myls -s tests
Error handling
./myls tests/not_found
echo $?

The program returns a non-zero exit status when an error occurs.
