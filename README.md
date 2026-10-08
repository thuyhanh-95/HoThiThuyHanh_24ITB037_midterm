# myls - System Programming Midterm

A simplified implementation of the NetBSD `ls(1)` command written in C for the System Programming midterm project.

---

## 1. Student Information

| Information | Details |
|---|---|
| **Name** | Ho Thi Thuy Hanh |
| **Student ID** | 24ITB037 |

---

## 2. Project Description

This project implements a simplified version of the NetBSD `ls(1)` command in the C programming language.

The program is named **`myls`** and is developed based on the provided **NetBSD 10.1 `ls(1)` manual**.

The main purpose of `myls` is to:

- List files and directories.
- Display detailed file information.
- Handle hidden files.
- Sort entries using different criteria.
- Display inode and block information.
- Support recursive directory traversal.
- Classify different file types.
- Handle non-printable characters.
- Process multiple command-line options.

When the operand is a file, the program displays information about that file.

When the operand is a directory, the program lists the contents of that directory.

When no operand is specified, the program lists the contents of the current directory.

---

## 3. Supported Options

The program supports the following options:

| Option | Description |
|---|---|
| `-A` | Display all entries except `.` and `..`. |
| `-a` | Include hidden entries whose names begin with `.`. |
| `-c` | Use file status change time instead of modification time. |
| `-d` | Display the directory itself instead of listing its contents. |
| `-F` | Append a symbol indicating the file type. |
| `-f` | Disable sorting. |
| `-h` | Display file sizes in human-readable format. |
| `-i` | Display the inode number. |
| `-k` | Display block sizes in kilobytes. |
| `-l` | Display information in long format. |
| `-n` | Display numeric UID and GID instead of owner and group names. |
| `-q` | Replace non-printable characters in file names with `?`. |
| `-R` | Recursively list subdirectories. |
| `-r` | Reverse the sorting order. |
| `-S` | Sort entries by file size, largest first. |
| `-s` | Display the number of file system blocks used. |
| `-t` | Sort entries by modification time, newest first. |
| `-u` | Use access time instead of modification time. |
| `-w` | Display non-printable characters in raw form. |

---

## 4. Usage

### General Syntax

```text
./myls [options] [file ...]
## 4.2 Basic Usage

List the current directory:

```sh
./myls
```

List the contents of the `tests` directory:

```sh
./myls tests
```

Display information about a specific file:

```sh
./myls tests/alpha.txt
```

## 4.3 Hidden Files

Display hidden files:

```sh
./myls -a tests
```

Display hidden files except `.` and `..`:

```sh
./myls -A tests
```

## 4.4 Long Format

Display detailed information:

```sh
./myls -l tests
```

Display human-readable sizes:

```sh
./myls -lh tests
```

Display numeric UID and GID:

```sh
./myls -n tests
```

## 4.5 Sorting

Sort by file size:

```sh
./myls -S tests
```

Sort by file size in reverse order:

```sh
./myls -Sr tests
```

Sort by modification time:

```sh
./myls -t tests
```

Reverse the sorting order:

```sh
./myls -tr tests
```

Disable sorting:

```sh
./myls -f tests
```

## 4.6 File Information

Display inode numbers:

```sh
./myls -i tests
```

Display block usage:

```sh
./myls -s tests
```

Display block usage in kilobytes:

```sh
./myls -sk tests
```

## 4.7 Recursive and Directory Modes

Recursively list subdirectories:

```sh
./myls -R tests
```

Display the directory itself:

```sh
./myls -d tests
```

## 4.8 File Classification

Display file type indicators:

```sh
./myls -F tests
```

The program uses the following indicators:

| Symbol | File Type       |
|--------|-----------------|
| `/`    | Directory       |
| `*`    | Executable file |
| `@`    | Symbolic link   |
| `\|`   | FIFO            |
| `=`    | Socket          |

## 4.9 Non-printable Characters

Replace non-printable characters with `?`:

```sh
./myls -q tests
```

Display non-printable characters in raw form:

```sh
./myls -w tests
```

## 4.10 Time Selection

Use modification time:

```sh
./myls -lt tests
```

Use access time:

```sh
./myls -ltu tests
```

Use status change time:

```sh
./myls -ltc tests
```

## 5. Project Structure

```text
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
    ├── .env_sample
    ├── alpha.txt
    ├── beta.log
    ├── huge.dat
    ├── middle.dat
    ├── notes.md
    ├── pipe_test
    ├── runme
    ├── shortcut
    ├── tiny.dat
    ├── docs/
    │   └── report.txt
    └── nested/
        └── level2/
            └── deep.txt
```

## 6. Module Description

### `main.c`

Responsible for:

- Program initialization.
- Processing command-line arguments.
- Handling file and directory operands.
- Calling the appropriate processing functions.

### `options.c`

Responsible for:

- Parsing command-line options.
- Storing option states.
- Handling option precedence.

### `listing.c`

Responsible for:

- Opening and reading directories.
- Filtering hidden files.
- Retrieving file metadata.
- Processing directory entries.
- Recursive directory traversal.

### `sorting.c`

Responsible for:

- Sorting entries by name.
- Sorting entries by file size.
- Sorting entries by time.
- Reversing the sorting order.
- Disabling sorting when `-f` is specified.

### `display.c`

Responsible for:

- Normal output.
- Long-format output.
- File size formatting.
- Inode display.
- Block display.
- Owner and group display.
- File classification.
- Symbolic-link target display.
- Non-printable character handling.

### `include/`

Contains the header files used by the source modules.

### `tests/`

Contains files and directories used for functional testing.

The test set includes regular files, hidden files, files with different sizes, an executable file, a symbolic link, a FIFO, nested directories, and a file containing a non-printable character.

## 7. Compilation

The project uses the provided Makefile.

Build the program:

```sh
make
```

Clean compiled files:

```sh
make clean
```

After successful compilation, the executable is:

```text
myls
```

## 8. Testing

The program was tested on NetBSD using the following test cases.

### 8.1 Basic Listing

```sh
./myls tests
```

### 8.2 Hidden Files

```sh
./myls -a tests
./myls -A tests
```

### 8.3 Long Format

```sh
./myls -l tests
./myls -lh tests
./myls -n tests
```

### 8.4 Inode and Block Information

```sh
./myls -i tests
./myls -s tests
./myls -sk tests
```

### 8.5 Sorting

```sh
./myls -S tests
./myls -Sr tests
./myls -t tests
./myls -tr tests
./myls -f tests
```

### 8.6 Recursive and Directory Modes

```sh
./myls -R tests
./myls -d tests
```

### 8.7 File Classification

```sh
./myls -F tests
```

### 8.8 Non-printable Characters

```sh
./myls -q tests
./myls -w tests
```

### 8.9 Time Selection

```sh
./myls -lt tests
./myls -ltu tests
./myls -ltc tests
```

### 8.10 Option Precedence

The following combinations were tested:

```sh
./myls -ln tests
./myls -nl tests
./myls -Rd tests
./myls -dR tests
./myls -qw tests
./myls -wq tests
./myls -ltuc tests
./myls -ltcu tests
```

For option pairs that override each other, the last specified option determines the final behavior.

### 8.11 BLOCKSIZE

The `BLOCKSIZE` environment variable was tested using:

```sh
BLOCKSIZE=1024 ./myls -s tests
```

and:

```sh
BLOCKSIZE=4096 ./myls -s tests
```

The block count changes according to the selected block size.

### 8.12 Error Handling

An invalid file path was tested:

```sh
./myls tests/not_found
```

Expected output:

```text
myls: tests/not_found: No such file or directory
```

The exit status was then checked:

```sh
echo $?
```

Expected result:

```text
1
```
