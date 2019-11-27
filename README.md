# CISC3160Project

The following defines a simple language, in which a program consists of assignments and each variable is assumed to be of the integer type. For the sake of simplicity, only operators that give integer values are included. Write an interpreter for the language in a language of your choice. Your interpreter should be able to do the following for a given program: (1) detect syntax errors; (2) report uninitialized variables; and (3) perform the assignments if there is no error and print out the values of all the variables after all the assignments are done.

 Program:
	Assignment*

Assignment:
	Identifier = Exp;

Exp: 
	Exp + Term | Exp - Term | Term

Term:
	Term * Fact  | Fact

Fact:
	( Exp ) | - Fact | + Fact | Literal | Identifier

Identifier:
     	Letter [Letter | Digit]*

Letter:
	a|...|z|A|...|Z|_

Literal:
	0 | NonZeroDigit Digit*
		
NonZeroDigit:
	1|...|9

Digit:
	0|1|...|9

***********************************************************************

### 1.1 Rewrite the grammer(eliminate the ambiguity and left recursion)
```
REWRITE:
E = TE'
E' = +TE' | -TE' | EOS
T = FT'
T' = *FT' | EOS
F = (E) | -F | +F | LI | ID
ID = LE ID'
ID' = LE ID'|DI ID' | EOS
LE = [ a-z, A-Z, _ ]
LI = 0 | NDS
NDS = ND DS
DS = DI DS | EOS
ND = [ 1-9 ]
DI = [ 0-9 ]
```
### 1.2 Find the FIRST set, FOLLOW set, and LL(1) SELECT set.
```
FIRST SET:
DI = { [ 0 - 9 ] }
ND = { [ 1-9] }
NDS = { [ 1-9] }
DS = { [ 0 - 9 ] }
LI = { [ 0 - 9 ] }
LE = { a|...|z|A|...|Z|_ }
ID' = { FIRST(LE)  FIRST(DI)  EOS }
ID = { FIRST(LE) }
F = { '('  '-'  '+'   FIRST(LI)   FIRST(ID) }
T' = { '*'   EOS }
T = { FIRST(F) }
E' = { '+'   '-'  EOS }
E = { FIRST(T) }

FOLLOW SET:
DI = { FIRST(DI)   FIRST(LE)   FOLLOW(DS) }
ND = { FIRST(DI)   FOLLOW(NDS) }
NDS = { FOLLOW(LI) }
DS = { FOLLOW(NDS) }
LI = { FOLLOW(F) }
LE = { FIRST(LE)   FIRST(DI)   FOLLOW(ID')   FOLLOW(ID) }
ID' = { FOLLOW(ID) }
ID = { FOLLOW(F) }
F = { FOLLOW(T') '+'  '*'}
T' = { FOLLOW(T) }
T = { '+'  '-'  FOLLOW(E)  FOLLOW(E') }
E' = { FOLLOW(E) }
E = { ')' }

SELECT SET:
DI = { [ 0 - 9 ] }
ND = { [ 1-9] }
NDS = { [ 1-9] }
DS = { [ 0 - 9 ] + FOLLOW(F) }
LI = { [ 0 - 9 ] }
LE = { a|...|z|A|...|Z|_ }
ID' = { FIRST(LE)    FIRST(DI)     FOLLOW(F) EOS }
ID = { FIRST(LE) }
F = { '('  '-'  '+'   FIRST(LI)   FIRST(ID) }
T' = { '*'  EOS   FOLLOW(T)}
T = { FIRST(F) }
E' = { '+'  '-'  EOS  FOLLOW(E) }
E = { FIRST(T) }
```

### 2.1 Separation
Split the assignment into identifier and expression, and check if assignment has '=' and ';'.
```
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
```

### 2.2 Parsing
The parsing is according to the grammar and FIRST, FOLLOW, AND SELECT sets.

```
char inputs[] = "---(x+y)*(x+-y)";
int index = -1;
char input_token = '\0';

const char PLUS = '+';
const char MINUS = '-';
const char MULTIPLICATION = '*';
const char DIVISION = '/';
const char L_PAREN = '(';
const char R_PAREN = ')';
const char EOS = '\0';

//function prototypes.
void error(string fooName);
void match(char expected_token);
void expe();
char next_token();
void exp_prime();
void term();
void term_prime();
void factor();
void ID();

int main()
{
    input_token = next_token();
    expe();
    if(input_token == EOS){
        cout<<"Match!"<<endl;
    }
    else{
        cout<<"Doesn't match!"<<endl;
    }

    return 0;
}

void match(char expected_token){
    if(input_token != expected_token){
        error("match()");
    }
    else{
        input_token = next_token();
    }
}

char next_token(){
    return inputs[++index];
}

bool DIGIT(){
    int num = (int)input_token;
    if(num>=48&&num<=57){
        return true;
    }
    return false;
}

bool NONZERODIGIT(){
    int num = (int)input_token;
    if(num>=49&&num<=57){
        return true;
    }
    return false;
}

bool LETTER(){
    int num = (int)input_token;
    if((num>=97&&num<=122)||(num>=65&&num<=90)||num==95){
        return true;
    }
    return false;
}

void DIGITS(){
    if(DIGIT()){
        match(input_token);
        DIGITS();
        return;
    }
    else if(input_token == EOS||input_token == MULTIPLICATION||
    input_token==PLUS||input_token == MINUS||input_token == R_PAREN){
        return;
    }
    error("DIGITS()");
    return;
}

void NONZEROBEGINDIGITS(){
    if(NONZERODIGIT()){
        match(input_token);
        DIGITS();
        return;
    }
    error("NONZEROBEGINDIGITS()");
    return;
}

void LITERAL(){
    if(DIGIT()){
        match(input_token);
        return;
    }
    error("LITERAL()");
    return;
}

void ID_prime(){
    if(LETTER()||DIGIT()){
        match(input_token);
        ID_prime();
        return;
    }
    else if(input_token == EOS||input_token == MULTIPLICATION||
    input_token == PLUS|| input_token == MINUS||
    input_token == R_PAREN){
        return;
    }
    error("ID_prime()");
    return;
}

void ID(){
    if(LETTER()){
        match(input_token);
        ID_prime();
        return;
    }
    error("ID()");
    return;
}

void factor(){
    if(input_token == MINUS || input_token == PLUS){
        match(input_token);
        factor();
        return;
    }
    else if(DIGIT()){
        LITERAL();
        return;
    }
    else if(LETTER()){
        ID();
        return;
    }
    else if(input_token == L_PAREN){
        match(input_token);
        expe();
        if(input_token == R_PAREN){
            match(input_token);
            return;
        }
    }
    error("factor()");
    return;
}

void term_prime(){
    if(input_token == MULTIPLICATION){
        match(MULTIPLICATION);
        factor();
        term_prime();
        return;
    }
    else if(input_token == EOS||input_token == MINUS||
    input_token == PLUS|| input_token == R_PAREN){
        return;
    }
    error("term_prime()");
    return;
}

void term(){
    if(input_token == L_PAREN|| DIGIT() || LETTER() ||
    input_token == PLUS || input_token == MINUS){
        factor();
        term_prime();
        return;
    }
    error("term()");
    return;
}

void expe_prime(){
    if(input_token == PLUS|| input_token == MINUS){
        match(input_token);
        term();
        expe_prime();
        return;
    }
    else if(input_token == EOS|| input_token == R_PAREN){
        return;
    }
    error("expe_prime()");
    return;
}

void expe(){
    if(input_token == L_PAREN|| DIGIT() || LETTER() ||
    input_token == PLUS || input_token == MINUS){
        term();
        expe_prime();
        return;
    }
    error("expe()");
    return;
}

void error(string fooName){
    cout<<"Doesn't match!"<<endl;
    cout<<"Error happend at function: "<<fooName<<endl;
    cout<<"At char: "<<input_token<<", index of string: "<<index<<endl;
    return;
}
```

### 2.3 Build syntax tree during the parsing.
Node's value is the operator, the leaves are the operands. Generate the syntax tree while parsing.
```
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
```

### 2.4 Syntax tree traversal by postorder traversal.
push node's value into stack
```
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
```

### 2.5 Evaluate expression
After got the reverse polish notation, program evaluate the expression and return result.
```
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
```

### 2.6 Save the pair(identifier,value) into map
Save the (identifier, value), it helps the parsing program and evaluation function to detect if a identifier is already declare or not.
```
//save pair<identifier,value> in map.
IDENTIFIER_MAP.insert({IDENTIFIER,result});
```

### 2.7 Output the identifier's value.
```
for(auto i = OUTPUTS.begin(); i != OUTPUTS.end(); i++){
        cout<<*i<<" = "<<*(i++)<<endl;
    }
```
