# myls

**Author:** Ho Thi Thuy Hanh  
**Student ID:** 24ITB037

---

## About

`myls` follows the behaviour described in the **NetBSD 10.1 `ls(1)` manual**, covering the most commonly used options rather than the full feature set.

What it can do:

- show files and directories, with or without extra detail
- reveal or hide dot-files
- order output by name, size, or time
- print inode numbers and block usage
- walk directory trees recursively
- mark entries by type
- deal safely with unprintable characters in names
- accept several options at once

How operands are treated:

| Operand | Result |
|---|---|
| none | contents of the current directory |
| a directory | contents of that directory |
| a file | information about that file only |

---

## Quick Start

Compile:

```sh
make
```

Remove build output:

```sh
make clean
```

The build produces a single executable called `myls`. Run it as:

```text
./myls [options] [file ...]
```

---

## Option Reference

### Which entries are shown

| Flag | Effect |
|---|---|
| `-a` | include entries starting with `.` |
| `-A` | like `-a`, but leave out `.` and `..` |
| `-d` | show a directory itself, not what is inside it |
| `-R` | descend into subdirectories |

### Ordering

| Flag | Effect |
|---|---|
| `-f` | no sorting at all |
| `-r` | reverse the current order |
| `-S` | largest files first |
| `-t` | newest files first |

### Which timestamp is used

| Flag | Effect |
|---|---|
| `-c` | status change time |
| `-u` | last access time |

Without either flag, the modification time is used.

### What is printed

| Flag | Effect |
|---|---|
| `-l` | long listing |
| `-h` | human-readable sizes |
| `-n` | numeric UID/GID rather than names |
| `-i` | inode number |
| `-s` | number of blocks used |
| `-k` | block counts in kilobytes |
| `-F` | type marker after each name |
| `-q` | show unprintable characters as `?` |
| `-w` | show unprintable characters unchanged |

---

## Examples

All examples use the `tests` directory that ships with the project.

### Everyday listing

| Command | What you get |
|---|---|
| `./myls` | the current directory |
| `./myls tests` | everything inside `tests` |
| `./myls tests/alpha.txt` | details of one file |

### Dot-files

| Command | What you get |
|---|---|
| `./myls -a tests` | all entries, including `.` and `..` |
| `./myls -A tests` | all entries except `.` and `..` |

### Long format

| Command | What you get |
|---|---|
| `./myls -l tests` | full details per entry |
| `./myls -lh tests` | the same, with readable sizes |
| `./myls -n tests` | numeric owner and group IDs |

### Sorting

| Command | What you get |
|---|---|
| `./myls -S tests` | sorted by size |
| `./myls -Sr tests` | sorted by size, smallest first |
| `./myls -t tests` | sorted by modification time |
| `./myls -tr tests` | sorted by time, oldest first |
| `./myls -f tests` | directory order, unsorted |

### Inodes and blocks

| Command | What you get |
|---|---|
| `./myls -i tests` | inode number before each name |
| `./myls -s tests` | block usage per entry |
| `./myls -sk tests` | block usage in kilobytes |

### Recursion and directory mode

| Command | What you get |
|---|---|
| `./myls -R tests` | `tests` and all subdirectories |
| `./myls -d tests` | only `tests` itself |

### Type markers

`./myls -F tests` appends one of these characters:

| Marker | Meaning |
|---|---|
| `/` | directory |
| `*` | executable file |
| `@` | symbolic link |
| `\|` | FIFO |
| `=` | socket |

### Unprintable characters

| Command | What you get |
|---|---|
| `./myls -q tests` | `?` in place of unprintable bytes |
| `./myls -w tests` | bytes printed as they are |

### Choosing the timestamp

| Command | What you get |
|---|---|
| `./myls -lt tests` | modification time |
| `./myls -ltu tests` | access time |
| `./myls -ltc tests` | status change time |

---

## Source Layout

| File | Role |
|---|---|
| `src/main.c` | startup, argument handling, deciding whether each operand is a file or a directory, and dispatching the work |
| `src/options.c` | reading flags, remembering which are active, and resolving conflicts between them |
| `src/listing.c` | reading directories, skipping hidden entries when needed, collecting metadata, and recursing into subdirectories |
| `src/sorting.c` | ordering by name, size or time, reversing, and skipping the sort for `-f` |
| `src/display.c` | all output: short and long formats, size formatting, inode and block columns, owner and group, type markers, link targets, and unprintable characters |
| `include/` | headers shared by the source files |
| `tests/` | sample files used for manual testing |

`tests` contains ordinary files, dot-files, files of different sizes, an executable, a symbolic link, a FIFO, nested directories, and a file with an unprintable character in its name.

---

## Verification

`myls` was checked by hand on NetBSD. Expand a group below to see the commands that were run.

<details>
<summary><b>Basic, hidden files, long format</b></summary>

```sh
./myls tests
./myls -a tests
./myls -A tests
./myls -l tests
./myls -lh tests
./myls -n tests
```

</details>

<details>
<summary><b>Inodes, blocks, sorting</b></summary>

```sh
./myls -i tests
./myls -s tests
./myls -sk tests
./myls -S tests
./myls -Sr tests
./myls -t tests
./myls -tr tests
./myls -f tests
```

</details>

<details>
<summary><b>Recursion, type markers, unprintable characters, timestamps</b></summary>

```sh
./myls -R tests
./myls -d tests
./myls -F tests
./myls -q tests
./myls -w tests
./myls -lt tests
./myls -ltu tests
./myls -ltc tests
```

</details>

<details>
<summary><b>Combined options</b></summary>

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

When two options conflict, the one given last wins.

</details>

<details>
<summary><b>BLOCKSIZE environment variable</b></summary>

```sh
BLOCKSIZE=1024 ./myls -s tests
BLOCKSIZE=4096 ./myls -s tests
```

The reported block counts change with the chosen block size.

</details>

<details>
<summary><b>Error handling</b></summary>

```sh
./myls tests/not_found
```

Expected message:

```text
myls: tests/not_found: No such file or directory
```

Then check the exit status:

```sh
echo $?
```

Expected value:

```text
1
```

</details>

