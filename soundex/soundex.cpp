
// This file contains functions which help the function "soundex" convert names to soundex code.
// This also contains soundexSearch which searches for similar names using the soundex codes. 


#include "testing/SimpleTest.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include <fstream>
#include <cctype>
#include <string>
using namespace std;

// This function takes a string s and returns a version of of the string which has all non letters removed
// The bug made it so that adjacent non-letters would not be accounted for. After the first non-letter was removed, an indexing
// problem meant that the second one would be skipped over. In order to fix this, I added "i--" after the removal of a character.
// Therefore, when we start testing isalpha(s[i]), we make sure that we start by testing the character next to the one removed.

string removeNonLetters(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (!isalpha(s[i])) {
            // remove the character at index i
            s = s.substr(0,i) + s.substr(i+1);
            i--;
        }
    }
    return s;
}

//this function takes a string of letters and uses the soundex table to convert them into a string of numbers.
//the string of numbers is then returned
string encodeDigits(string s){
    string encoded = "";
    for (char letter : s){
        letter = toupper(letter);
        if ((letter == 'A') || (letter == 'E') || (letter == 'I') || (letter == 'O')
           || (letter == 'U') || (letter == 'H') || (letter == 'W') || (letter == 'Y')){
            encoded += "0";
        }
        else if ((letter == 'B') || (letter == 'F') || (letter == 'P') || (letter == 'V')){
            encoded += "1";
        }
        else if ((letter == 'C') || (letter == 'G') || (letter == 'J') || (letter == 'K') ||
                (letter == 'Q') || (letter == 'S') || (letter == 'X') || (letter == 'Z')){
            encoded += "2";
        }
        else if ((letter == 'D') || (letter == 'T')){
            encoded += "3";
        }
        else if (letter == 'L'){
            encoded += "4";
        }
        else if ((letter == 'M') || (letter == 'N')){
            encoded += "5";
        }
        else if (letter == 'R'){
            encoded += "6";
        }
    }
    return encoded;
}


//this function checks if there are groups of the same number next to each other. regardless of how many adjacent
//same numbers there are, this function will get rid of the duplicates. it takes in a string of numbers and returns
//the string without duplicates and unpadded
string coalesceSimilar(string s){
    for(int i = 0; i < s.length(); i++){
        if (s.substr(i, 1).compare(s.substr(i+1, 1)) == 0){
            for (int j = i+1; j < s.length(); j++){     // finds the next non-duplicate and removes duplicates
                if (s.substr(j, 1).compare(s.substr(i, 1)) != 0){
                    s = s.substr(0, i) + s.substr(i, 1) + s.substr(j);
                    break;
                }
                if ((s.substr(j, 1).compare(s.substr(i, 1)) == 0) && (j == s.length() -1)){ //edge case where duplicates run towards the end. (ex. the string ends with "777")
                    s = s.substr(0, i) + s.substr(i, 1);
                    break;
                }

            }
        }
    }
    return s;
}


//this function truncates or pads on zeros if the length is greater than or less than four.
//it takes in a string and returns a string that is padded
string makeLengthFour(string s){
    if (s.length() > 4){
        s = s.substr(0, 4);
    }
    else if (s.length() < 4){
        int zerosNeeded = 4 - s.length();
        for (int i = 0; i < zerosNeeded; i++){
            s.append("0");
        }
    }
    return s;
}

//this function iterates through a string and removes all zeros. it takes in a string and returns
//a string without zeros
string removeZeros(string s){
    for (int i = 0; i < s.length(); i++) {
        if (s.substr(i, 1).compare("0") == 0) {
            s = s.substr(0,i) + s.substr(i+1);
            i--;
        }
    }
    return s;
}


//this function takes a string and outputs its Soundex code using the helper functions removeNonLetters,
//encodeDigits, coalesceSimilar, removeZeros, and makeLengthFour. the helper functions are run in the order
//specified by the Soundex algorithm and the Soundex code of the input is returned as a string
string soundex(string s) {
    s = removeNonLetters(s);
    char fLetter = s[0];
    s = encodeDigits(s);
    s = coalesceSimilar(s);
    s[0] = toupper(fLetter);
    s = removeZeros(s);
    s = makeLengthFour(s);
    return s;
}


//this function opens a file and reads it onto a vector. the user is prompted for a name and then all names in the file with a matching
//soundex will be printed as well as the soundex code. the function continues to run until the user types RETURN to quit. nothing
//is returned
void soundexSearch(string filepath) {
    // The proivded code opens the file with the given name 
    // and then reads the lines of that file into a vector.
    ifstream in;
    Vector<string> databaseNames;

    if (openFile(in, filepath)) {
        readEntireFile(in, databaseNames);
    }
    cout << "Read file " << filepath << ", "
         << databaseNames.size() << " names found." << endl;
    // The names in the database are now stored in the provided vector named databaseNames

    bool isRunning = true;
    while (isRunning){
        string name = getLine("Enter a surname (RETURN to quit): ");
        if (name.compare("") == 0){ //if the user hits RETURN, the while loop ends
            cout << "All done!";
            isRunning = false;
        }
        else{
            string soundexCode = soundex(name);
            cout << "Soundex code is " << soundexCode << endl << "\n";

            Vector<string> matchingNames;
            for (int i = 0; i < databaseNames.size(); i++){
                if (soundex(databaseNames.get(i)).compare(soundexCode) == 0){
                    matchingNames.add(databaseNames.get(i));  //finds names with matching codes are adds them to the vector
                }
            }

            matchingNames.sort();
            cout << "Matches from database: " << matchingNames << endl << "\n";
        }
    }


}


/* * * * * * Test Cases * * * * * */


PROVIDED_TEST("Test removing puntuation, digits, and spaces") {
    string s = "O'Hara";
    string result = removeNonLetters(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "tldr");
    s = "_bug_192a__"; //test that I added per assignment instructions
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "buga");
}


PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Master"), "M236");
    EXPECT_EQUAL(soundex("Jue"), "J000");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}



STUDENT_TEST("test removenonletters with many nonletters"){
    EXPECT_EQUAL(removeNonLetters("__Jackson_--"), "Jackson");
    EXPECT_EQUAL(removeNonLetters("123B2o_o__on"), "Booon");

}

STUDENT_TEST("test encodeDigits with various names"){
    EXPECT_EQUAL(encodeDigits("Daniels"), "3050042");
    EXPECT_EQUAL(encodeDigits("brennan"), "1605505");
    EXPECT_EQUAL(encodeDigits("hAttIe"), "003300");
}

STUDENT_TEST("testing coalesce with varying amouints of adjacent numbers"){
    EXPECT_EQUAL(coalesceSimilar("000777666"), "076");
    EXPECT_EQUAL(coalesceSimilar("3266444"), "3264");
    EXPECT_EQUAL(coalesceSimilar("165789999"), "165789");

}

STUDENT_TEST("testing coalesce with no adjacent same numbers"){
    EXPECT_EQUAL(coalesceSimilar("1234"), "1234");


}

STUDENT_TEST("testing makeLengthFour"){
    EXPECT_EQUAL(makeLengthFour("1232186"), "1232");
    EXPECT_EQUAL(makeLengthFour("2"), "2000");

}

STUDENT_TEST("testing removeZeros"){
    EXPECT_EQUAL(removeZeros("160160651000"), "1616651");
    EXPECT_EQUAL(removeZeros("165"), "165");
}


STUDENT_TEST("test Soundex with no input"){
    EXPECT_EQUAL(soundex(""), "0000");

}

STUDENT_TEST("test Soundex with a long input with non letters"){
    EXPECT_EQUAL(soundex("Oaweouih-wehoq-Auwwk"), "O220");

}

STUDENT_TEST("test Soundex with three duplicate codes"){
    EXPECT_EQUAL(soundex("aeioubfmnr"), "A156");

}

STUDENT_TEST("test Soundex with only duplicate code 2"){
    EXPECT_EQUAL(soundex("CZXSQQGJ"), "C000");

}

STUDENT_TEST("test Soundex with only one letter"){
    EXPECT_EQUAL(soundex("_r_"), "R000");

}
