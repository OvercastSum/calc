# CLI Calculator

A command-line calculator built in C++.
Reads mathematical expressions from input files and writes results to output files.

## Features
- Addition, subtraction, multiplication, division, power
- Variables
- Base conversion (e.g. decimal to binary)

## Compilation

### Visual Studio

### g++
```bash
g++ -std=c++17 *.cpp -o calculator
```

## Usage

### Example command
```bash
calculator -i ../examples/input.txt -o ../examples/output.txt -s 10 -S 10
```
### Input file format
Variable start with `$M` followed by a number:
$M5 = 2+2 <br />
$M3 = $M / 73

### Flags
| Flag | Description |
|------|-------------|
| `-i <file>` | Input file |
| `-o <file>` | Output file |
| `-s <base>` | Input base |
| `-S <base>` | Output base |
