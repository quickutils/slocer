
# slocer

Count the number of source line of code in a project or source file. Know the total number of line of code in a project or source file. You can download the latest release [here](https://github.com/quickutils/slocer/releases).

### Legends

- LOC - Line Of Code
- SLOC - Source Line of Code (Exempting Empty LOC)

## Compilation

To build the program you need to have the C compiler installed, any C compiler will do the task. You have to install the following libraries libxtd, libfio, libcline. Use the following command to install the exotic libraries.

### Windows

open powershell as admin and execute.

```powershell
& $([scriptblock]::Create((New-Object Net.WebClient).DownloadString("https://exoticlibraries.github.io/magic/install.ps1"))) libxtd libfio libcline
```

### Linux

open bash as su to install in */usr/include* else it installed in */usr/local/include*

```bash
bash <(curl -s https://exoticlibraries.github.io/magic/install.sh) libxtd libfio libcline
```

Example compilation command is at the top of the file.

## Usage

To view the list of accepted argument add the -h flag

```
slocer -h
```

Output:

```
usage: slocer [-h|--help] [source ...] [--ext=<file_type> ...] [-v|--verbose] [-r|--recurse]

Count the number of line in your project or source file, This does not give regard to comment.
MIT License Copyright (c) 2021, Adewale Azeez

  -h, --help           Print this help message
  --ext=<file_type> ... The file extension to count it lines
  -v, --verbose        Print verbose detail in the terminal
  -r, --recurse        Recursively count the lines in file and sub folder recursively
```

Specify a folder or file to count it LOC and SLOC. If a folder is specified the lines of the files in that folder will be counted, the sub folder will be ignored. Specify the flag `-r` or `--recurse` to walk the subfolders. If a file is specified the lines of code in that file will be counted.

To select the file extension to count it lines, use the `--ext=` argument. You can specify the `--ext=` argument more than once for reading files with different extension.

### Example

```
slocer -r ./libxtd/include ./libfio/include --ext=.c --ext=.h
```

Output

```
Total File: 22
Total (LOC): 8944
Total (SLOC): 7609
```

## Contribution

You can send PR. It a One time project so long support. Create Github issue if it worth my time I ll attend to it else I ll close it. Clone it and extend it if you want.

## Useful Links

- [Exotic Libraries](https://exoticlibraries.github.io/)
- [Exotic Libraries Source](https://github.com/exoticlibraries/)

## Licence

MIT License Copyright (c) 2021, Adewale Azeez
