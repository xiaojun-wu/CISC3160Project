#define __USE_MINGW_ANSI_STDIO 0
#include <iostream>
#include <vector>
#include <string.h>
#include <stack>
#include <unordered_map>

using namespace std;

//set some global variables

//char inputs[] = {'(','3','+','4',')','*','5'};
//char inputs[] = {'-','-','-','(','x','+','y',')','\0'};
//char inputs[] = {'-','-','-','(','x','+','y',')','*','(','x','+','-','y',')','\0'};
//char inputs[] = "---(x+y)*(x+-y)";
string INPUT = "---(x+y)*(x+-y)";
string IDENTIFIER;
int IND = -1;
char input_token = '\0';

//dynamic arrays
//RPN is each expression's reverse polish notation.
//OUPUTS is for store each evaluated statement.
vector<string> RPN;
vector<string> OUTPUTS;

const char PLUS = '+';
const char MINUS = '-';
const char MULTIPLICATION = '*';
const char DIVISION = '/';
const char L_PAREN = '(';
const char R_PAREN = ')';
const char EOS = '\0';
//IDENTIFIER_MAP is store the pair<identifier,value>
//LINE indicate which line of assignment is reading.
unordered_map<string, int> IDENTIFIER_MAP;
int LINE = 0;

//node struct for building a syntax tree.
struct node{
    string value = "";
    node *left = nullptr;
    node *right = nullptr;
};

//function prototypes.
void error(string fooName, string reason);
void match(char expected_token);
node* expe();
char next_token();
node* exp_prime();
node* term();
node* term_prime();
node* factor();
node* ID();
void postOrder(node *root);
int evaluation(stack <int> operands);
void sliptAssiment(string &identifier, string &expression, const string assigment);
void eliminateSpaceSemicolon(string &s);
node* generateSyntaxTree();
node* verifyStatement(string identifier, string expression);

int main()
{
    //dynamic array for store each unevaluated assigment.
    vector<string> assigments;
    stack <int> operands;
    string input;
    string statement;
    string expression;
    int result;
    
    cout<<"Enter your assigments, enter a blank line to end program.\n";
    do{
        getline(cin,input);
        if(input != ""){
            assigments.push_back(input);
        }
    }while(input != "");

    //evaluate each line of assignment.
    for(auto ptr = assigments.begin(); ptr < assigments.end(); ptr++){
        LINE++;
        statement = *ptr;
        eliminateSpaceSemicolon(statement);
        //split assigment into identifier and expression
        sliptAssiment(IDENTIFIER,expression,statement);
        node *syntaxTree;
        //generate a syntaxtree
        syntaxTree = verifyStatement(IDENTIFIER, expression);
        //generate the RNP by using postorder traversal.
        postOrder(syntaxTree);
        // for(auto i = RPN.begin(); i != RPN.end(); i++){
        //     cout<<*i;
        // }
        //cout<<endl;
        
        //evaluate the expression.
        result = evaluation(operands);
        //save pair<identifier,value> in map.
        IDENTIFIER_MAP.insert({IDENTIFIER,result});
        delete syntaxTree;
        //save the identifier and it's value in OUTPUTS stack.
        OUTPUTS.push_back(to_string(result));
        OUTPUTS.push_back(IDENTIFIER);
        RPN.clear();
        if(operands.size() != 0 ){
            operands.pop();
        }
    }
    
    for(auto i = OUTPUTS.begin(); i != OUTPUTS.end(); i++){
        cout<<*i<<" = "<<*(i++)<<endl;
    }
    return 0;
}

//check if the current char is match expected char, and eat that.
void match(char expected_token){
    if(input_token != expected_token){
        error("match()","input doesn't match.");
    }
    else{
        input_token = next_token();
    }
}

//eat a token.
char next_token(){
    return INPUT[++IND];
}

//check if the char is a digit or not.
bool DIGIT(){
    int num = (int)input_token;
    if(num>=48&&num<=57){
        return true;
    }
    return false;
}

//check if the char is zero.
bool ZERO(){
    int num = (int)input_token;
    if(num == 48){
        return true;
    }
    return false;
}

//check if the char is non-zero digits.
bool NONZERODIGIT(){
    int num = (int)input_token;
    if(num>=49&&num<=57){
        return true;
    }
    return false;
}

//check if the char is a letter.
bool LETTER(){
    int num = (int)input_token;
    if((num>=97&&num<=122)||(num>=65&&num<=90)||num==95){
        return true;
    }
    return false;
}

//keep reading a char until the new char isn't a digit, return the digits.
node* DIGITS(){
    node *parent;
    string digits = "";
    while(DIGIT()){
        digits += input_token;
        match(input_token);
    }
    if(input_token == EOS||input_token == MULTIPLICATION||
    input_token==PLUS||input_token == MINUS||input_token == R_PAREN){
        parent = new node;
        parent->value = digits;
        return parent;
    }
    error("DIGITS()", "expect operator, R_PAREN, or EOS.");
    return nullptr;
}

//check if the char is begin with non-zero digit.
node* NONZEROBEGINDIGITS(){
    node *parent;
    if(NONZERODIGIT()){
        parent = DIGITS();
        return parent;
    }
    error("NONZEROBEGINDIGITS()", "expect a non zero digit.");
    return nullptr;
}

node* LITERAL(){
    node *parent;
    if(ZERO()){
        parent = new node;
        parent->value = input_token;
        match(input_token);
        return parent;
    }
    else{
        parent = NONZEROBEGINDIGITS();
        return parent;
    }
    error("LITERAL()", "expect a digit.");
    return nullptr;
}

//check if it's a valid id, if the id is not present before
//and if this id isn't this assignment's identifier, return error.
node* ID_prime(){
    string id = "";
    while(LETTER()||DIGIT()){
        id += input_token;
        match(input_token);
    }
    if(input_token == EOS||input_token == MULTIPLICATION||
    input_token == PLUS|| input_token == MINUS||
    input_token == R_PAREN){
        if(IDENTIFIER_MAP.find(id) == IDENTIFIER_MAP.end() && id != IDENTIFIER){
            error("ID_prime()", "uninitialized Identifier: "+id);
        }
        node *parent = new node;
        parent->value = id;
        return parent;
    }
    error("ID_prime()", "expect a operator, letter, digit, R_PAREN or EOS.");
    return nullptr;
}

//check if it is a id.
node* ID(){
    if(LETTER()){
        node *parent = ID_prime();
        return parent;
    }
    error("ID()", "expect a letter");
    return nullptr;
}

//check if it is a factor.
node* factor(){
    node *parent = new node;
    if(input_token == MINUS || input_token == PLUS){
        parent->value = input_token;
        match(input_token);
        node *left = new node;
        left->value = "0";
        parent->left = left;
        parent->right = factor();
        return parent;
    }
    else if(DIGIT()){
        parent->left = LITERAL();
        return parent;
    }
    else if(LETTER()){
        parent->left = ID();
        return parent;
    }
    else if(input_token == L_PAREN){
        match(input_token);
        parent->left = expe();
        if(input_token == R_PAREN){
            match(input_token);
            return parent;
        }
        else{
            error("factor()", "expect a R_PAREN.");
        }
    }
    error("factor()", "expect operator, digit, letter, L_PAREN.");
    return nullptr;
}

//check if it is a * symbol.
node* term_prime(){
    if(input_token == MULTIPLICATION){
        node *parent = new node;
        parent->value = input_token;
        match(MULTIPLICATION);
        parent->left = factor();
        parent->right = term_prime();
        return parent;
    }
    else if(input_token == EOS||input_token == MINUS||
    input_token == PLUS|| input_token == R_PAREN){
        return nullptr;
    }
    error("term_prime()", "expect operator, R_PAREN, or EOS.");
    return nullptr;
}

node* term(){
    if(input_token == L_PAREN|| DIGIT() || LETTER() ||
    input_token == PLUS || input_token == MINUS){
        node *parent = new node;
        parent->left = factor();
        parent->right = term_prime();
        return parent;
    }
    error("term()", "expect L_PAREN, digit, letter, +, or -.");
    return nullptr;
}

//if the current char is a + or - operator,
//assign it as node's value,
//assign next factor as right child,
//if the next char is - or +,
//this node will be the operator's left child.
/*
    a - b + c
    
      -      +                                            +
     / \    / \    if operator != nullptr,               / \
    a   b      c                                        -   c
                                                       / \
                                                      a   b
*/
node* expe_prime(node *&root){
    if(input_token == PLUS|| input_token == MINUS){
        node *parent = new node;
        root = parent;
        parent->value = input_token;
        match(input_token);
        parent->right = term();
        node *expPrime = expe_prime(root);
        if(expPrime != nullptr){
            expPrime->left = parent;
        }
        return parent;
    }
    else if(input_token == EOS|| input_token == R_PAREN){
        return nullptr;
    }
    error("expe_prime()", "expect +, -, R_PAREN or EOS.");
    return nullptr;
}

node* expe(){
    if(input_token == L_PAREN|| DIGIT() || LETTER() ||
    input_token == PLUS || input_token == MINUS){
        node *parent = new node;
        node *root = parent;
        parent->left = term();
        node *expPrime = expe_prime(root);
        if(expPrime != nullptr){
            expPrime->left = parent;
        }
        return root;
    }
    error("expe()", "expect digit, letter, +, -, or L_PAREN.");
    return nullptr;
}

//if current char is unexpected, report error.
void error(string fooName,string reason){
    cout<<"Syntax error!"<<endl;
    cout<<"Error happanded at line : "<<LINE<<", at funtion: "<<fooName<<endl;
    cout<<reason<<endl;
    return exit(0);
}

// traverse syntaxtree by using postorder traversal.
void postOrder(node *root){
    if(root == nullptr){
        return;
    }
    postOrder(root->left);
    postOrder(root->right);
    if(root->value != ""){
        RPN.push_back(root->value);
    }
    return;
}

//evaluated the RNP and return it's value.
int evaluation(stack <int> operands){
    int operand1, operand2, value;
    unordered_map<string, int>::iterator iter;

    for(auto i = RPN.begin(); i != RPN.end();){
        if(*i == "+"||*i == "-"||*i == "*"){
            operand1 = operands.top();
            operands.pop();
            operand2 = operands.top();
            operands.pop();
            string operator1 = *i;
            value = 0;
            if(operator1 == "+"){
                value = operand2 + operand1;
            }
            else if(operator1 == "-"){
                value = operand2 - operand1;
            }
            else{
                value = operand2 * operand1;
            }
            operands.push(value);
        }
        else if((iter = IDENTIFIER_MAP.find(*i)) != IDENTIFIER_MAP.end()){
            operands.push(iter->second);
        }
        else{
            operands.push(stoi(*i,nullptr,10));
        }
        i++;
    }
    if(operands.size() == 1){
        return operands.top();
    }
    else{
        cout<<"RPN is wrong!"<<endl;
        return 0;
    }
}

// eliminate the space and last char.
void eliminateSpaceSemicolon(string &s){
    string ss;
    if(s[s.size()-1] != ';'){
        error("eliminateSpaceSemicolon", "expect semicolon at the end.");
    }
    for(int i = 0; i < s.size()-1; i++){
        if(s[i] != ' '){
            ss += s[i];
        }
    }
    s = ss;
}

// split the assigment into two parts, and check if identifier is not empty.
void sliptAssiment(string &identifier, string &expression, const string assigment){
    int index = assigment.find("=");
    identifier = assigment.substr(0,index);
    expression = assigment.substr(index+1,assigment.size()-index-1);
    if(identifier.size() == 0){
        error("sliptAssiment()","miss identifier.");
    }
    else if(expression.empty() || index == -1){
        error("sliptAssiment()", "identifier: "+identifier+" is uninitialized.");
    }
}

// generate syntaxtree
node* generateSyntaxTree(){
    IND = -1;
    input_token = next_token();
    node *syntaxTree;
    syntaxTree = expe();
    if(input_token != EOS){
        error("End of statement","unexpected symbol at the end.");
    }
    return syntaxTree;
}

// check identifier and expression are valid syntax.
node* verifyStatement(string identifier, string expression){
    node *ptr;
    INPUT = identifier;
    ptr = generateSyntaxTree();
    delete ptr;
    INPUT = expression;
    ptr =  generateSyntaxTree();
    return ptr;
}
