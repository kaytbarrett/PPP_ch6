
//
// This is example code from Chapter 6.7 "Trying the second version" of
// "Software - Principles and Practice using C++" by Bjarne Stroustrup
//

/*
    This program implements a basic expression calculator.
    Input from cin, output to cout.
    The grammar for input is:

    Statement: 
        Expression
        Print
        Quit
    Print: 
        ";"
    Quit:
        "q"
    Expression:
        Term
        Expression "+" Term
        Expression "-" Term
    Term:
        Primary
        Term "*" Primary
        Term "/" Primary
        Term "%" Primary
    Primary:
        Number
        "(" Expression ")"
        "-" Primary
        "+" Primary
    Number:
        Floating-point-literal

*/

#include "std_lib_facilities.h"

//------------------------------------------------------------------------------

class Token{
public:

    static constexpr char number = '8'; // Define the constant at the top

    char kind;        // what kind of token
    double value;     // for numbers: a value 

    // Constructor to create a Token from a char
    Token(char ch)    
        :kind(ch), value(0) { }

    // Constructor to create a Token from a char and a double
    Token(char ch, double val)     
        :kind(ch), value(val) { }
};

constexpr char quit = 'q';
constexpr char print = ';';
constexpr char prompt = '>';
constexpr char result = '=';


//------------------------------------------------------------------------------

class Token_stream {
public:
    Token_stream();   // make a Token_stream that reads from cin
    Token get();      // get a Token (get() is defined elsewhere)
    void putback(Token t);    // put a Token back
private:
    bool full;        // is there a Token in the buffer?
    Token buffer;     // here is where we keep a Token put back using putback()
};

//------------------------------------------------------------------------------

// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream()
    :full(false), buffer(0)    // no Token in buffer
{
}

//------------------------------------------------------------------------------

// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t)
{
    if (full) error("putback() into a full buffer");
    buffer = t;       // copy t to buffer
    full = true;      // buffer is now full
}

//------------------------------------------------------------------------------

Token Token_stream::get()
{
    if (full) {       // do we already have a Token ready?
        // remove token from buffer
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch;    // note that >> skips whitespace (space, newline, tab, etc.)

    switch (ch) {
    case print:    
    case quit:    
    case '(': 
    case ')': 
    case '+': 
    case '-': 
    case '*': 
    case '/': 
    case '{': 
    case '}': 
    case '!': 
    case '%':
        return Token(ch);        // let each character represent itself
    case '.':                    // a floating point literal can start with a .
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
        cin.putback(ch);         // put digit back into the input stream
        double val;
        cin >> val;              // read a floating-point number
        return Token(Token::number, val);   
    }
    default:
        error("Bad token");
    }
}

//------------------------------------------------------------------------------

Token_stream ts;        // provides get() and putback() 

//------------------------------------------------------------------------------

double expression();    // declaration so that primary() can call expression()

//------------------------------------------------------------------------------

// deal with numbers and parentheses
double primary()
{
    Token t = ts.get();
    switch (t.kind) {
    case '(':    // handle '(' expression ')'
    {
        double d = expression();
        t = ts.get();
        if (t.kind != ')') error("')' expected");
        return d;
    }
    case '{':    // handle '{' expression '}'
    {
        double d = expression();
        t = ts.get();
        if (t.kind != '}') error("'}' expected");
        return d;
    }
    case Token::number:            
    {
        int left = t.value;
        t = ts.get();

        if (t.kind == '!')
        {
            if (left < 0) error("Factorial of a negative number is undefined");

            int result = 1;
            for(int i = left; i > 1; --i )
            {
            result *= i;
            }
            left = result;
        } else {
            ts.putback(t);  // put the token back if it's not '!'
        }

        return left;  // return the number's value
    }
    case '-':
        return - primary();
    case '+':
        return primary();
    default:
        error("primary expected");
    }
}

//------------------------------------------------------------------------------

// deal with *, /, and %
double term()
{
    double left = primary();
    Token t = ts.get();        // get the next token from token stream

    while (true) {
        switch (t.kind) {
        case '*':
        {
            double d = primary();
            left *= d;
            t = ts.get();
            break;
        }
        case '/':
        {
            double d = primary();
            if (d == 0) error("divide by zero");
            left /= d;
            t = ts.get();
            break;
        }
        case '%':
        {
            double d = primary();
            if (d == 0) error("%: divide by zero");
            left = fmod(left, d);
            t = ts.get();
            break;
        }
        default:
            ts.putback(t);     // put t back into the token stream
            return left;
        }
    }
}

//------------------------------------------------------------------------------

// deal with + and -
double expression()
{
    double left = term();      // read and evaluate a Term
    Token t = ts.get();        // get the next token from token stream

    while (true) {
        switch (t.kind) {
        case '+':
            left += term();    // evaluate Term and add
            t = ts.get();
            break;
        case '-':
            left -= term();    // evaluate Term and subtract
            t = ts.get();
            break;
        default:
            ts.putback(t);     // put t back into the token stream
            return left;       // finally: no more + or -: return the answer
        }
    }
}

//------------------------------------------------------------------------------

void calculate()
{
    while (cin) {
        cout << prompt;
        Token t = ts.get();

        while (t.kind == print)
            t = ts.get();
        if (t.kind == quit ) 
            return;

        ts.putback(t);
        cout << result << expression() << endl;
    }

}

int main()
try
{
    cout << "\nWelcome to our simple calculator!\n" <<
    "\nPlease enter expressions using floating-point numbers.\n" <<
    "For this program, you can use (), +, -, *, and /.\n" <<
    "To end the expression you want to calculate, end with an = sign.\n" <<
    "For example: (2+3)*11;\n\n" <<
    "To exit, please enter q.\n\n";

    calculate();
    return 0;

}
catch (exception& e) {
    cerr << "error: " << e.what() << '\n';
    keep_window_open();
    return 1;
}
catch (...) {
    cerr << "Oops: unknown exception!\n";
    keep_window_open();
    return 2;
}

//------------------------------------------------------------------------------