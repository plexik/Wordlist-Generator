# Wordlist-Generator
Command line application that takes a list of words and uses them to generate a dictionary for use in targetted dictionary attacks.

Usage:
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

This tool was created with the intent to ease the production of dictionaries for use in password attacks against a researched target.
The input to the program is a list (one word per line) of "initial seed" words.  The words in this input list should be relevant to
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
