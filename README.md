# CLI Calculator

A command-line calculator built in C++.
Reads mathematical expressions from input files and writes results to output files.

## Features
- Addition, subtraction, multiplication, division, power
- Variables
- Works on files

## Usage

### Example command
```bash
calculator -i input.txt -o output.txt -s 10 -S 10
```
### Input file format
Variable start with `$M` followed by a number:
$M5 = 2+2
$M3 = $M / 73

### Flags
| Flag | Description |
|------|-------------|
| `-i <file>` | Input file |
| `-o <file>` | Output file |
| `-s <base>` | Input base |
| `-S <base>` | Output base |
