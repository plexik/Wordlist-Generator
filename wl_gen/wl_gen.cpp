// Wordlist Generator for dictionary attacks against a researched target
// plexik 2016

#define VERSION_NUM "1.00"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstddef>
#include <cstring>

int capitalizeFirstLetter(std::string* word);
int capitalizeAllLetters(std::string* word);
int reverseWord(std::string* word);

int main(int argc, const char* argv[]) {

    // input and output file streams
    std::ifstream words_infile;
    std::ofstream words_outfile;

    // the output dictionary
    std::vector<std::string> words;

    // temporary arrays used to build the final dictionary
    std::string* seeds;     // initial seed array used to build the extended seed array
    std::string* ext_seeds; // extended seed array used in permutations

    // word counts for the seeds and ext_seeds arrays
    unsigned num_seeds;
    unsigned num_ext_seeds;

    // often-used 'buffer' variables
    std::string buf;    // single-word buffer used throughout program
    std::stringstream word_builder; // string stream used to build permutations

    // argument parsing variables
    std::string infile_name = "";
    std::string outfile_name = "";
    bool verbose = false;
    bool quiet = false;

    const std::string help_message = "\n"
        "Jeff's Wordlist Generator " VERSION_NUM "\n\n"
        "Usage:\twl_gen [options] -i infile -o outfile\n\n"
        "Required:\n"
        "\t-i infile\tinput filename (use '-' for stdin)\n"
        "\t-o outfile\toutput filename (use '-' for stdout)\n\n"
        "Options:\n"
        "\t-h or -?\tthis help message\n"
        "\t-v\t\tverbose output\n"
        "\t-q\t\tquiet mode - no status messages (overwrites -v)\n";

    ///////////////////////////////
    // argument parsing
    ///////////////////////////////
    //
    // ./wl_gen [options] -i infile -o outfile
    // 
    // required:
    //      -i infile       input filename (use '-' for stdin)
    //      -o outfile      output filename (use '-' for stdout)
    //
    // options:
    //      -h or -?        help
    //      -v              verbose output
    //      -q              quiet mode - no status messages (overwrites -v)


    //std::clog << argc << " arguments detected" << std::endl;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) { // verbose output
            if (!quiet)
                verbose = true;
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0) { // help message
            std::clog << help_message << std::endl;
            return 0;
        }
        else if (strcmp(argv[i], "-q") == 0) { // quiet mode
            verbose = false;
            quiet = true;
        }
        else if (strcmp(argv[i], "-i") == 0) { // input file flag
            if (i <= argc - 2) { // make sure there's at least one following argument
                if (argv[i + 1][0] == '-'){
                    if (strlen(argv[i + 1]) >= 2) { // starts with a - and is longer than 1 character = switch
                        std::cerr << "\nerror - missing filename after -i" << std::endl;
                        std::clog << help_message << std::endl;
                        return -1;
                    }
                }
                infile_name = argv[i + 1];
            }
            else {
                std::cerr << "\nerror - missing filename after -i" << std::endl;
                std::clog << help_message << std::endl;
                return -1;
            }
        }
        else if (strcmp(argv[i], "-o") == 0) { // output file flag
            if (i <= argc - 2) { // make sure there's at least one following argument
                if (argv[i + 1][0] == '-'){
                    if (strlen(argv[i + 1]) >= 2) { // starts with a - and is longer than 1 character = switch
                        std::cerr << "\nerror - missing filename after -o" << std::endl;
                        std::clog << help_message << std::endl;
                        return -1;
                    }
                }
                outfile_name = argv[i + 1];
            }
            else {
                std::cerr << "\nerror - missing filename after -o" << std::endl;
                std::clog << help_message << std::endl;
                return -1;
            }
        }
        else { // not an option
            // if the preceding argument is -i or -o then this must be the infile or outfile name
            if (strcmp(argv[i - 1], "-i") == 0 || strcmp(argv[i - 1], "-o") == 0) {
                continue;
            }
            std::cerr << "\nerror - unknown option " << argv[i] << std::endl;
            std::clog << help_message << std::endl;
            return -1;
        }
    }
    
    //std::clog << "\nparsed options:\nverbose:\t" << (verbose ? "true" : "false") << "\nquiet:\t\t" << (quiet ? "true" : "false") << "\ninfile:\t\t" << infile_name << "\noutfile:\t" << outfile_name << std::endl;

    if (infile_name == "") {
        std::cerr << "\nerror - input flag -i required" << std::endl;
        std::clog << help_message << std::endl;
        return -1;
    }
    if (outfile_name == "") {
        std::cerr << "\nerror - output flag -o required" << std::endl;
        std::clog << help_message << std::endl;
        return -1;
    }


    ///////////////////////////////
    // read input words
    ///////////////////////////////

    // (all lower case input assumed)
    if (!quiet) {
        std::clog << "\nJeff's Wordlist Generator " << VERSION_NUM << "\n" << std::endl;
    }
    if (verbose) {
        std::clog << "Starting..." << std::endl;
        std::clog << "  Loading input words...  ";
    }

    if (infile_name == "-") { // if reading from stdin (piped input)
        while (std::cin >> buf) {
            words.push_back(buf);
        }
    }
    else { // if reading from a file
        words_infile.open(infile_name.c_str(), std::ifstream::in);
        if (words_infile.fail()) {
            std::cerr << "\nerror opening " << infile_name << std::endl;
            return -2;
        }
        while (words_infile >> buf) {
            words.push_back(buf);
        }
        words_infile.close();
    }

    if (verbose) {
        std::clog << "done." << std::endl;
    }


    ///////////////////////////////
    // generate initial seed array
    ///////////////////////////////

    if (verbose) {
        std::clog << "  Generating initial seed array..." << std::endl;
    }

    num_seeds = words.size();

    seeds = new std::string[num_seeds];
    for (unsigned i = 0; i < num_seeds; i++){
        seeds[i] = words[i];
        //std::cout << "word " << i << ": " << seeds[i] << std::endl;
    }
    if (!quiet) {
        std::clog << "Initial Seed Array generated. (" << num_seeds << " words)" << std::endl;
    }


    ///////////////////////////////
    // generate extended seed array
    ///////////////////////////////
    //
    // words added <= 4 * num_seeds
    //

    if (verbose) {
        std::clog << "  Operating on initial seed array..." << std::endl;
        std::clog << "    Capitalize first letter...  ";
    }

    // first letter capitalized
    for (unsigned i = 0; i < num_seeds; i++) {
        buf = seeds[i];
        if (capitalizeFirstLetter(&buf) == 0) {
            words.push_back(buf);
        }
    }

    if (verbose) {
        std::clog << "done." << std::endl;
        std::clog << "    Capitalize whole word...  ";
    }

    // all capitalized
    for (unsigned i = 0; i < num_seeds; i++) {
        buf = seeds[i];
        if (capitalizeAllLetters(&buf) == 0) {
            words.push_back(buf);
        }
    }

    if (verbose) {
        std::clog << "done." << std::endl;
        std::clog << "    Reverse word...  ";
    }

    // reversed
    for (unsigned i = 0; i < num_seeds; i++) {
        buf = seeds[i];
        if (reverseWord(&buf) == 0) {
            words.push_back(buf);
        }
    }

    if (verbose) {
        std::clog << "done." << std::endl;
        std::clog << "  Generating extended seed array..." << std::endl;
    }

    delete[] seeds;

    // build extended seed array
    num_ext_seeds = words.size();

    ext_seeds = new std::string[num_ext_seeds];
    for (unsigned i = 0; i < num_ext_seeds; i++) {
        ext_seeds[i] = words[i];
    }
    if (!quiet) {
        std::clog << "Extended Seed Array generated. (" << num_ext_seeds << " words)" << std::endl;
    }


    ///////////////////////////////
    // extended seed array variations
    ///////////////////////////////
    //
    // words added <= 4203 * num_ext_seeds
    //

    if (verbose) {
        std::clog << "  Performing single-word variations..." << std::endl;
        std::clog << "    Append 0-2099...  ";
    }

    // append 0-2099
    for (unsigned i = 0; i < num_ext_seeds; i++) {
        for (unsigned j = 0; j < 2100; j++) {
            word_builder.str("");
            word_builder << ext_seeds[i] << j;
            words.push_back(word_builder.str());
        }
    }

    if (verbose) {
        std::clog << "done." << std::endl;
        std::clog << "    Prepend 0-2099...  ";
    }

    // prepend 0-2099
    for (unsigned i = 0; i < num_ext_seeds; i++) {
        for (unsigned j = 0; j < 2100; j++) {
            word_builder.str("");
            word_builder << j << ext_seeds[i];
            words.push_back(word_builder.str());
        }
    }

    if (verbose){
        std::clog << "done." << std::endl;
        std::clog << "    Append '!'...  ";
    }

    // append '!'
    for (unsigned i = 0; i < num_ext_seeds; i++) {
        word_builder.str("");
        word_builder << ext_seeds[i] << "!";
        words.push_back(word_builder.str());
    }

    if (verbose){
        std::clog << "done." << std::endl;
        std::clog << "    Prepend '!'...  ";
    }

    // prepend '!'
    for (unsigned i = 0; i < num_ext_seeds; i++) {
        word_builder.str("");
        word_builder << "!" << ext_seeds[i];
        words.push_back(word_builder.str());
    }

    if (verbose){
        std::clog << "done." << std::endl;
        std::clog << "    Append and prepend '!'...  ";
    }

    // append and prepend '!'
    for (unsigned i = 0; i < num_ext_seeds; i++) {
        word_builder.str("");
        word_builder << "!" << ext_seeds[i] << "!";
        words.push_back(word_builder.str());
    }

    if (verbose) {
        std::clog << "done." << std::endl;
    }
    if (!quiet){
        std::clog << "Single-Word Variations done. (" << words.size() << " words)" << std::endl;
    }


    ///////////////////////////////
    // permutations of two extended seed words
    ///////////////////////////////
    //
    // words added = 3 * nPr(num_ext_seeds, 2)
    //

    if (verbose) {
        std::clog << "  Performing two-word permutations..." << std::endl;
    }

    unsigned long perm_start_idx = words.size();

    for (unsigned i = 0; i < num_ext_seeds - 1; i++) {
        for (unsigned j = i + 1; j < num_ext_seeds; j++) {
            // forward permutations
            // ab
            word_builder.str("");
            word_builder << ext_seeds[i] << ext_seeds[j];
            words.push_back(word_builder.str());
            // a b
            //word_builder.str("");
            //word_builder << ext_seeds[i] << " " << ext_seeds[j];
            //words.push_back(word_builder.str());
            // a_b
            word_builder.str("");
            word_builder << ext_seeds[i] << "_" << ext_seeds[j];
            words.push_back(word_builder.str());
            // a.b
            word_builder.str("");
            word_builder << ext_seeds[i] << "." << ext_seeds[j];
            words.push_back(word_builder.str());

            // backward permutations
            // ba
            word_builder.str("");
            word_builder << ext_seeds[j] << ext_seeds[i];
            words.push_back(word_builder.str());
            // b a
            //word_builder.str("");
            //word_builder << ext_seeds[j] << " " << ext_seeds[i];
            //words.push_back(word_builder.str());
            // b_a
            word_builder.str("");
            word_builder << ext_seeds[j] << "_" << ext_seeds[i];
            words.push_back(word_builder.str());
            // b.a
            word_builder.str("");
            word_builder << ext_seeds[j] << "." << ext_seeds[i];
            words.push_back(word_builder.str());
        }
    }

    unsigned long perm_end_idx = words.size();

    if (!quiet) {
        std::clog << "Two-Word Permutations done. (" << words.size() << " words)" << std::endl;
    }

    delete[] ext_seeds;


    ///////////////////////////////
    // two-word permutation variations
    ///////////////////////////////
    //
    // words added = 23 * (3 * nPr(num_ext_seeds, 2))
    //

    if (verbose) {
        std::clog << "  Performing two-word permutation variations..." << std::endl;
        std::clog << "    Append '!'...  ";
    }

    // append '!'
    for (unsigned long i = perm_start_idx; i < perm_end_idx; i++) {
        word_builder.str("");
        word_builder << words[i] << "!";
        words.push_back(word_builder.str());
    }

    if (verbose) {
        std::clog << "done." << std::endl;
        std::clog << "    Prepend '!'...  ";
    }

    // prepend '!'
    for (unsigned long i = perm_start_idx; i < perm_end_idx; i++) {
        word_builder.str("");
        word_builder << "!" << words[i];
        words.push_back(word_builder.str());
    }

    if (verbose) {
        std::clog << "done." << std::endl;
        std::clog << "    Append and prepend '!'...  ";
    }

    // append and prepend '!'
    for (unsigned long i = perm_start_idx; i < perm_end_idx; i++) {
        word_builder.str("");
        word_builder << "!" << words[i] << "!";
        words.push_back(word_builder.str());
    }

    if (verbose) {
        std::clog << "done." << std::endl;
        std::clog << "    Append 0-9...  ";
    }

    // append 0-9
    for (unsigned long i = perm_start_idx; i < perm_end_idx; i++) {
        for (unsigned j = 0; j < 10; j++) {
            word_builder.str("");
            word_builder << words[i] << j;
            words.push_back(word_builder.str());
        }
    }

    if (verbose) {
        std::clog << "done." << std::endl;
        std::clog << "    Prepend 0-9...  ";
    }

    // prepend 0-9
    for (unsigned long i = perm_start_idx; i < perm_end_idx; i++) {
        for (unsigned j = 0; j < 10; j++) {
            word_builder.str("");
            word_builder << j << words[i];
            words.push_back(word_builder.str());
        }
    }

    if (verbose) {
        std::clog << "done." << std::endl;
    }
    if (!quiet) {
        std::clog << "Two-Word Permutation Variations done. (" << words.size() << " words)" << std::endl;
    }


    ///////////////////////////////
    // write wordlist
    ///////////////////////////////
    if (!quiet) {
        std::clog << "Writing " << words.size() << " words to " << (outfile_name == "-" ? "stdout" : outfile_name) << "...  ";
    }

    if (outfile_name == "-") {
        for (unsigned long i = 0; i < words.size(); i++){
            std::cout << words[i] << std::endl;
        }
        std::cout.flush();
    }
    else {
        words_outfile.open(outfile_name.c_str(), std::ofstream::out);
        if (words_outfile.fail()) {
            std::cerr << "\nerror opening " << outfile_name << " for writing" << std::endl;
            return -2;
        }

        for (unsigned long i = 0; i < words.size(); i++){
            words_outfile << words[i] << std::endl;
        }

        words_outfile.flush();
        words_outfile.close();
    }

    if (!quiet) {
        std::clog << "done." << std::endl;
    }

    return 0;
}


// returns 0 if successful, -1 if there were no letters to capitalize
int capitalizeFirstLetter(std::string* word) {
    std::size_t tmp = (*word).find_first_of("abcdefghijklmnopqrstuvwxyz");
    //std::clog << "\t first letter of " << (*word) << " found at index " << tmp << std::endl;
    if (tmp != std::string::npos){
        (*word)[tmp] = (char)toupper((*word)[tmp]);
        return 0;
    }
    else { // if there are no letters in the word
        return -1;
    }
}

// returns 0 if successful, -1 if there were no letters to capitalize
int capitalizeAllLetters(std::string* word) {
    int tmp = -1;
    // should use string iterator?
    for (unsigned i = 0; i < (*word).size(); i++) {
        if (isalpha((*word)[i])) {
            (*word)[i] = (char)toupper((*word)[i]);
            tmp = 0;
        }
    }

    return tmp;
}

// always returns 0 unless it's a single character
int reverseWord(std::string* word) {
    if ((*word).size() > 1) {
        std::stringstream tmp;
        tmp.str("");
        for (std::string::reverse_iterator rit = (*word).rbegin(); rit != (*word).rend(); ++rit) {
            tmp << *rit;
        }
        *word = tmp.str();
        return 0;
    }
    return -1;
}