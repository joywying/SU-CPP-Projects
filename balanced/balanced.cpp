//two helper functions work together to find out whether operations are balanced in a given string
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "testing/SimpleTest.h"

using namespace std;

//this function takes in a string and returns the string without any non-operator characters
//operators are defined as (), {}, or []
string operatorsOnly(string s) {
    string brackets = "()[]{}";
    if (s == ""){
        return "";
    }
    else if (brackets.find(s[0]) == string::npos) {
            return operatorsOnly(s.substr(1));

    }
    else{
        return s[0] + operatorsOnly(s.substr(1));
    }

}




//this function takes in a string of operators only and returns true if
//it is balanced, and false if it is not. checkOperators uses recursion
//to remove pairs of operators in order to verify the operators are balanced
//if this is called with non-operator characters, it will return false
bool checkOperators(string s) {
    int index = -1;
    if(s.find("()") != string::npos){
        index = s.find("()");
    }
    else if(s.find("[]") != string::npos){
        index = s.find("[]");
    }
    else if(s.find("{}") != string::npos){
        index = s.find("{}");
    }

    if (s == ""){
        return true;
    }
    else if (index >= 0){
        s = s.substr(0, index) + s.substr(index + 2); //removes pair of operators
        return checkOperators(s);
    }

    return false;

}


/* 
 * This function assumes correct implementation of the previous 
 * two functions. It uses the operatorsOnly function to strip out
 * all characters from the provided string and then checks to see
 * whether the remaining operators are balanced by using the 
 * checkOperators function. All in all, this function is able to 
 * correctly determine whether a snippet of code has correctly
 * balanced bracketing operators. You should not need to modify this
 * function if the previous two functions have been implemented
 * correctly. 
 */
bool isBalanced(string str) {
    string ops = operatorsOnly(str);
    return checkOperators(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsOnly on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    string only = "(){([])(())}";
    EXPECT_EQUAL(operatorsOnly(example), only);
}

PROVIDED_TEST("checkOperators on example from writeup") {
    string only = "(){([])(())}";
    EXPECT(checkOperators(only));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on illegal examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}

STUDENT_TEST("operatorsOnly only operators"){
    EXPECT_EQUAL(operatorsOnly("())({}{}[][()]][]]]}{"), "())({}{}[][()]][]]]}{");
}

STUDENT_TEST("operatorsOnly on balanced and unbalanced inputs"){
    string input = "{H101__)aO{";
    EXPECT_EQUAL(operatorsOnly(input), "{){");
    input = "{a + wsi} + ((b) * (d))";
    EXPECT_EQUAL(operatorsOnly(input), "{}(()())");

}

STUDENT_TEST("checkOperators on input with non-operators"){
    string input = "{H101__)aO{";
    EXPECT(!checkOperators(input));
    input = "hello";
    EXPECT(!checkOperators(input));
    input = "";
    EXPECT(checkOperators(input));

}

STUDENT_TEST("checkOperators on short unbalanced input"){
    string input = ")}";
    EXPECT(!checkOperators(input));
    input = ")(";
    EXPECT(!checkOperators(input));

}

STUDENT_TEST("checkOperators on balanced inputs"){
    string input = "([([()])])";
    EXPECT(checkOperators(input));
    input = "{{{}}}[]([])({})";
    EXPECT(checkOperators(input));
}

STUDENT_TEST("checkOperators with one character"){
    string input = "[";
    EXPECT(!checkOperators(input));
}

STUDENT_TEST("isBalanced with no operators and another balanced input"){
    string input = "hello test";
    EXPECT(isBalanced(input));
    input = "([([()])])";
    EXPECT(isBalanced(input));
}
