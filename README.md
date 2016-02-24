# Wordlist-Generator
Command line application that takes a list of words and uses them to generate a dictionary for use in targeted dictionary attacks.
This tool was created with the intent to ease the production of dictionaries for use in password attacks against a researched target.

## Usage
```
./wl_gen [options] -i infile -o outfile

required:
    -i infile       input filename (use '-' for stdin)
    -o outfile      output filename (use '-' for stdout)
    
 options:
    -h or -?        help
    -v              verbose output
    -q              quiet mode - no status messages (overwrites -v)
```

### Math
As of version 1.00, an input list of ten words (no numbers) yields an output dictionary of 280480 words.

Given N input words, the resulting dictionary size will be less than or equal to
```
<= 4204 (4N) + 24 ( 3 nPr(4N, 2))
= 16816 N + 24 (3 16N^2 - 4N)
= 1152N^2 + 16528N
```
So don't go passing in 100K word dictionaries - it probably won't end well.

## Features
- fast
- supports input and output piping
- compiles on both linux and windows

## How it works

The input to the program is a list of "initial seed" words.  The words in this input list should be relevant to
the target, such as first and last name, birth year, first dog's name, etc.  Basically it's a list of strings you think could be used in a password.

The program will then apply the following operations to the initial seeds to generate an extended seed array:
- (do nothing)
- Capitalize first letter
- Capitalize entire word
- Reverse word

Next it performs the following operations on each word in the extended seed array:
- Append 0-2099
- Prepend 0-2099
- Append '!'
- Prepend '!'
- Append and prepend '!'

Next it generates all possible two-word permutations and combines them the following ways:
- ab
- a_b
- a.b
- ba
- b_a
- b.a

Finally, to each of these permutations it performs the following:
- append '!'
- repend '!'
- append and prepend '!'
- append 0-9
- prepend 0-9

Output is then written to a file (or stdout).
