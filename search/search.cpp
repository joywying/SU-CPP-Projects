//Helper functions work together to create an inverted index for a file containing
//URLs and text and allow users to search for words

#include "testing/SimpleTest.h"
#include "map.h"
#include "set.h"
#include <string>
#include <iostream>
#include "filelib.h"
#include "float.h"
#include <fstream>
#include "simpio.h"
#include "strlib.h"
#include "search.h"
using namespace std;

//this function takes a string token and trims all of the punctuation from the
//beginning and end according to the ispunct function. If the token does not
//contain at least one letter, an empty string is returned and all cleaned tokens
//are converted to lowercase before being returned
string cleanToken(string token) {
    bool containsAtleastOneLetter = false;
    for (char character : token){
        if (isalpha(character)) {
            containsAtleastOneLetter = true;
            break;
        }
    }

    if (ispunct(token[0])) { //removes punctuation from the beginning
        for(int i = 1; i < token.length(); i++){
            if (!ispunct(token[i])) {
                token = token.substr(i);
                break;
            }
        }
    }

    if (ispunct(token[token.length() -1])) { //removes punctuation from the end
        for (int i = token.length() -1; i >= 0; i--){
            if (!ispunct(token[i])) {
                token = token.substr(0, i+1);
                break;
            }
        }
    }

    if (containsAtleastOneLetter == true) {
        return toLowerCase(token);
    }
    else{
        return "";
    }

}

//this function takes a file and reads the contents onto a vector of lines
//all URLs are read from the Vector<string> and added to a map as a key
//each URL or key is associated with a Set<string> of cleaned tokens.
//the map containing the URLs as keys and the set of cleaned tokens is returned
Map<string, Set<string>> readDocs(string dbfile) {
    Map<string, Set<string>> docs;
    ifstream in;

    if (!openFile(in, dbfile)) {
        error("Cannot open file named " + dbfile);
    }

    Vector<string> lines;
    readEntireFile(in, lines);

    for(int i = 0; i < lines.size(); i++){
        if (i % 2 == 0) {
            Set<string> allTokens;
            Vector<string> pageContent = stringSplit(lines.get(i+1), " ");

            for (string word : pageContent){
                if (cleanToken(word) != "") {
                    allTokens.add(cleanToken(word)); //adding all cleaned words to a set
                }
            }
            docs[lines.get(i)] = allTokens; //adding the URL and cleaned tokens into the map
        }
    }

    return docs;
}

//this function creates an inverted index given a map with URLs as the keys and sets of words
//associated with those keys. A map containing words as the keys and a set of URLs associated with
//each word is returned
Map<string, Set<string>> buildIndex(Map<string, Set<string>>& docs) {
    Map<string, Set<string>> index;

    for (string key : docs){
        for(string word : docs[key]){
            index[word];      //finds a word and then adds it to our index as a key
            for (string URL : docs){ //looks through all our Sets in docs. if a word is in the Set, the URL associated with that set is added to the index entry for the word
                if (docs[URL].contains(word)) {
                    index[word].add(URL);
                }
            }
        }

    }

    return index;
}

//this function takes a map, which is the inverted index, and a query
//and returns a set of URL matches for the given query. Querys can
//use '-', '+' and spaces to indicate whether invidivdual search matches
//should be unioned, intersected, or differenced
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) {
    Set<string> result;

    Vector<string> terms = stringSplit(query, " ");
    result += index[cleanToken(terms.get(0))];

    for(int i = 1; i < terms.size(); i++){
        string currentTerm = terms.get(i);

        if (isalpha(currentTerm[0])) {
            result = result + index[cleanToken(currentTerm)];
        }
        else if (currentTerm[0] == '+') {
            result = result * index[cleanToken(currentTerm.substr(1))];
        }
        else if (currentTerm[0] == '-') {
            result = result - index[cleanToken(currentTerm.substr(1))];
        }
    }

    return result;
}

//this function uses helper functions readDocs, buildIndex, and findQueryMatches
//to allow the user to see how many URLs are processed from a file, how many distinct words
//are found across all content, and find URL matches for queries. The file name is taken
//as the argument and the function returns nothing
void searchEngine(string dbfile) {
    Map<string, Set<string>> readFile = readDocs(dbfile);
    cout << "Stand by while building index..." << endl;
    Map<string, Set<string>> index = buildIndex(readFile);


    cout << "Indexed " << readFile.size() << " pages containing " << index.size() << " unique terms." << endl;
    bool isRunning = true;
    while (isRunning){
        string searchTerm = getLine("Enter query sentence (RETURN/ENTER to quit): ");
        if (searchTerm == "") {
            cout << "All done!";
            isRunning = false;
        }
        else{
            Set<string> allMatches = findQueryMatches(index, searchTerm);
            cout << "Found " << allMatches.size() << " matching page(s)" << endl << allMatches << endl << "\n";
        }


    }

}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on tokens with no punctuation") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
}

PROVIDED_TEST("cleanToken on tokens with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
}

PROVIDED_TEST("cleanToken on non-word tokens"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}

PROVIDED_TEST("readDocs from tiny.txt, contains 4 documents") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    EXPECT_EQUAL(docs.size(), 4);
}

PROVIDED_TEST("readDocs from tiny.txt, suess has 5 unique words and includes lowercase fish") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Set<string> seuss = docs["www.dr.seuss.net"];
    EXPECT_EQUAL(seuss.size(), 5);
    EXPECT(seuss.contains("fish"));
    EXPECT(!seuss.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 20 unique tokens overall") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);

    EXPECT_EQUAL(index.size(), 20);
    EXPECT(index.containsKey("fish"));
    EXPECT(!index.containsKey("@"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 3);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}

//STUDENT TESTS

STUDENT_TEST("cleanToken on tokens with some punctuation at beginning and end and middle") {
    EXPECT_EQUAL(cleanToken("-!!didn't!-"), "didn't");
    EXPECT_EQUAL(cleanToken("~?as-is!"), "as-is");
}

STUDENT_TEST("readDocs from tiny.txt, shoppinglist has 5 words") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Set<string> list = docs["www.shoppinglist.com"];
    EXPECT_EQUAL(list.size(), 5);

}

STUDENT_TEST("readDocs from tiny.txt, rainbow has 7 unique words"){
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Set<string> list = docs["www.rainbow.org"];
    EXPECT_EQUAL(list.size(), 7);
}

STUDENT_TEST("readDocs from tiny.txt, bigbadwolf has 6 words, one of them is 'i'm'"){
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Set<string> list = docs["www.bigbadwolf.com"];
    EXPECT_EQUAL(list.size(), 6);
    EXPECT(list.contains("i'm"));
}

STUDENT_TEST("buildIndex from tiny.txt should give us two URLs associated with 'blue' and 'fish'"){
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);

    EXPECT_EQUAL(index["blue"].size(), 2);
    EXPECT_EQUAL(index["fish"].size(), 2);
}


STUDENT_TEST("buildIndex from tiny.txt. 'i'm' should be associated with one URL"){
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);
    Set<string> oneLink;
    oneLink.add("www.bigbadwolf.com");
    EXPECT_EQUAL(index["i'm"], oneLink);
}

STUDENT_TEST("findQueryMatches compound queries. Various tests to ensure searching is not case sensitive and that the '+' and '-' operators work") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);

    Set<string> matchesBlueOrBreadNotGreen = findQueryMatches(index, "BLUE bread -green");
    EXPECT_EQUAL(matchesBlueOrBreadNotGreen.size(), 2);
    Set<string> matchesGreenAndIndigoNotOrange = findQueryMatches(index, "green +inDIgo -orange");
    EXPECT_EQUAL(matchesGreenAndIndigoNotOrange.size(), 0);
    Set<string> matchesFishNotBlue = findQueryMatches(index, "fish -blue");
    EXPECT_EQUAL(matchesFishNotBlue.size(), 1);
    Set<string> matchesYellowNotMilkNotYou = findQueryMatches(index, "yellow -milk -you");
    EXPECT_EQUAL(matchesYellowNotMilkNotYou.size(), 1);
}
