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
