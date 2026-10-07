# myls - System Programming Midterm

## 1. Student Information

- **Name:** Ho Thi Thuy Hanh 
- **Student ID:** 24ITB037

## 2. Project Description

This project implements a simplified version of the NetBSD `ls(1)` command in C.

The program is named **`myls`** and is developed according to the provided **NetBSD 10.1 `ls(1)` manual**.

Supported options:
-A	List all entries except . and ...
-a	Include entries whose names begin with ..
-c	Use status change time instead of modification time.
-d	Display the directory itself instead of its contents.
-F	Append a character indicating the file type.
-f	Do not sort the output.
-h	Display sizes in human-readable format.
-i	Display the inode number.
-k	Display sizes in kilobytes.
-l	Display information in long format.
-n	Display numeric UID and GID.
-q	Replace non-printable characters with ?.
-R	Recursively list subdirectories.
-r	Reverse the sorting order.
-S	Sort by file size, largest file first.
-s	Display the number of file system blocks used.
-t	Sort by modification time, newest file first.
-u	Use access time instead of modification time.
-w	Print non-printable characters in raw form.

## 3. Usage

./myls [options] [file]
