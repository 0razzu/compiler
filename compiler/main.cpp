#include "globals.hpp"
#include "stack.hpp"
#include "stack.cpp"
#include <iostream>
#include <fstream>
#include <sstream>


u_short priority(char o) {
    u_short p = 0;
    
    if ((o == 'e') || (o == 'f'))
        p = 7;
    
    else if ((o == '^') || (o == '~') || (o == 'p') || (o == 'm'))
        p = 6;
    
    else if (o == '*')
        p = 5;
    
    else if ((o == '/') || (o == '&'))
        p = 4;
    
    else if ((o == '+') || (o == '-'))
        p = 3;
    
    else if ((o == '<') || (o == '>') || (o == '=') || (o == '#'))
        p = 2;
    
    else if (o == '!')
        p = 1;
    
    return p;
}


bool associativity(char o) {
    bool left = 1;
    
    if ((o == '^') || (o == '~') || (o == 'p') || (o == 'm') || (o == '/') || (o == '&') || (o == '>') || (o == '<') || (o == '=') || (o == '#'))
        left = 1;
    
    else if ((o == '*') || (o == '!') || (o = 'e') || (o = 'f'))
        left = 0;
    
    return left;
}


bool read_num(std::istream &in, u_long &curr, char num[16]) {
    bool ok = 0, first_dot = 1;
    char c = '\0', next = '0';
    u_long curr_before_reading = curr;
    
    next = in.peek();
    
    if (((next >= '0') && (next <= '9')) || (next == '.') || (next == ',')) {
        for (u_short i = 0; i < 16; i++)
            num[i] = '\0';
        
        ok = 1;
    }
    
    while ((ok) && (((next >= '0') && (next <= '9')) || (next == '.') || (next == ','))) {
        in.get(c);
        curr++;
        
        if ((first_dot) && ((c == '.') || (c == ',')))
            first_dot = 0;
        
        else if ((c >= '0') && (c <= '9')) {
        }
        
        else
            ok = 0;
        
        if (ok) {
            num[curr - curr_before_reading - 1] = c;
            next = in.peek();
            
            if ((curr - curr_before_reading == 15) && (((next >= '0') && (next <= '9')) || (next == '.') || (next == ','))) {
                ok = 0;
                curr++;
            }
        }
    }
    
    if ((!first_dot) && (curr - curr_before_reading == 1))
        ok = 0;
    
    return ok;
}


bool read_word(std::istream &in, u_long &curr, const char word[], ushort word_length) {
    bool ok = 1, matches = 1;
    ushort i = 0;
    char c, next;
    
    while ((i < word_length) && (matches)) {
        next = in.peek();
        
        matches = (word[i] == next);
        
        if (matches) {
            in.get(c);
            curr++;
        }
            
        i++;
    }
        
    ok = matches;
    
    return ok;
}


uint8_t recognizer(std::istream &in, bool from_file, char &wrong_symbol, u_long &curr, u_long &curr_line, std::stringstream &rpn) {
    bool ok = 1, prev_lexeme_is_num = 0, add_space = 0, o = 0;
    const char v_true[5] = "TRUE", v_false[6] = "FALSE", f_frac[5] = "frac", f_entier[7] = "entier";
    char next, c = '\0', num[16], operator_in_stack;
    uint8_t state = OK;
    Stack<char> operators;
    
    curr = 0;
    curr_line = 1;
    
    next = in.peek();
    
    while ((state == OK) && (((from_file) && (in.peek() != EOF)) || ((!from_file) && (in.peek() != '\n')))) {
        if ((next == ' ') || (next == '\t')) {
            in.get(c);
            curr++;
        }
        
        else if (next == '\n') {
            in.get(c);
            curr_line++;
            curr = 0;
        }
        
        else if (((next >= '0') && (next <= '9')) || (next == '.') || (next == ',')) {
            ok = read_num(in, curr, num);
            
            if (ok) {
                if (add_space)
                    rpn << ' ';
                
                rpn << num;
                
                prev_lexeme_is_num = 1;
                add_space = 1;
            }
            
            else
                state |= UNEXPECTED;
        }
        
        else if (next == 'B') {
            in.get(c);
            curr++;
            
            if (add_space)
                rpn << ' ';
            
            next = in.peek();
            
            if ((next == '0') || (next == '1')) {
                in.get(c);
                curr++;
                
                if (next == '0')
                    rpn << 'F';
                
                else
                    rpn << 'T';
            }
            
            else if (((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
                rpn << c;
                
                while (((next >= '0') && (next <= '9')) || ((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
                    in.get(c);
                    curr++;
                    rpn << c;
                    next = in.peek();
                }
            }
            
            else
                state |= UNEXPECTED;
            
            prev_lexeme_is_num = 0;
            add_space = 1;
        }
        
        else if (next == 'R') {
            in.get(c);
            curr++;
            next = in.peek();
            
            if (add_space)
                rpn << ' ';
            
            if (((next >= '0') && (next <= '9')) || (next == '.') || (next == ',')) {
                ok = read_num(in, curr, num);
                
                if (ok)
                    rpn << num;
            }
            
            else if (((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
                rpn << 'R';
                
                while (((next >= '0') && (next <= '9')) || ((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
                    in.get(c);
                    curr++;
                    rpn << c;
                    next = in.peek();
                }
            }
                
            else
                state |= UNEXPECTED;
            
            prev_lexeme_is_num = 1;
            add_space = 1;
        }
        
        else if (((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
            ok = 0;
            
            if (next == 'T') {
                ok = read_word(in, curr, v_true, 4);
                
                if (ok)
                    rpn << 'T';
            }
            
            else if (next == 'F') {
                ok = read_word(in,curr, v_false, 5);
                
                if (ok)
                    rpn << 'F';
            }
            
            else if (next == 'f') {
                ok = read_word(in, curr, f_frac, 4);
                
                if (ok) {
                    c = 'f';
                    o = 1;
                }
            }
            
            else if (next == 'e') {
                ok = read_word(in, curr, f_entier, 6);
                
                if (ok) {
                    c = 'e';
                    o = 1;
                }
            }
            
            if (!ok)
                state |= UNEXPECTED;
        }
        
        else if (next == '+') {
            in.get(c);
            curr++;
            
            if (!prev_lexeme_is_num)
                c = 'p';
            
            o = 1;
        }
        
        else if (next == '-') {
            in.get(c);
            curr++;
            
            if (!prev_lexeme_is_num)
                c = 'm';
            
            o = 1;
        }
        
        else if ((next == '*') || (next == '/') || (next == '^') ||
                 (next == '~') || (next == '&') || (next == '!') ||
                 (next == '<') || (next == '>') || (next == '=') || (next == '#')) {
            in.get(c);
            curr++;
            
            o = 1;
        }
        
        else if (next == '(') {
            in.get(c);
            curr++;
            
            operators.push(c);
            
            prev_lexeme_is_num = 0;
        }
        
        else if (next == ')') {
            in.get(c);
            curr++;
            
            while ((!operators.isEmpty()) && (operators.peek() != '(')) {
                operator_in_stack = operators.pop();
                rpn << operator_in_stack;
                add_space = 0;
            }
            
            if (operators.isEmpty())
                state |= BRACKETS;
            else
                operators.del();
            
            prev_lexeme_is_num = 1;
        }
        
        else
            state |= UNEXPECTED;
        
        while ((state == OK) && (o)) {
            if ((operators.isEmpty()) || (operators.peek() == '(')) {
                operators.push(c);
                o = 0;
                prev_lexeme_is_num = 0;
            }
            
            else if ((!operators.isEmpty()) && (priority(c) > priority(operators.peek()))) {
                operators.push(c);
                o = 0;
                prev_lexeme_is_num = 0;
            }
            
            else if ((!operators.isEmpty()) && (priority(c) == priority(operators.peek())) && (!associativity(c))) {
                operators.push(c);
                o = 0;
                prev_lexeme_is_num = 0;
            }
            
            else {
                operator_in_stack = operators.pop();
                rpn << operator_in_stack;
                add_space = 0;
            }
        }
        
        next = in.peek();
        
        if (in.fail())
            state |= READ_ERR;
    }
    
    if (state == OK)
        while ((!operators.isEmpty()) && (state == OK)) {
            operator_in_stack = operators.pop();
            
            if (operator_in_stack == '(')
                state |= BRACKETS;
            
            else
                rpn << operator_in_stack;
        }
    
    if (state & UNEXPECTED) {
        curr++;
        wrong_symbol = next;
    }
    
    return state;
}


bool generate_code(std::stringstream &rpn, std::ostream &code) {
    bool ok = 1;
    char next, c;
    
    code << '{' << std::endl;
    
    next = rpn.peek();
    
    while ((!rpn.eof()) && (ok)) {
        if ((next == 'B') || (next == 'R')) {
            code << "\tPSH ";
            
            while (((next >= '0') && (next <= '9')) || ((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
                rpn.get(c);
                code << c;
                next = rpn.peek();
            }
            
            code << std::endl;
        }
        
        else if (((next >= '0') && (next <= '9')) || (next == '.') || (next == ',')) {
            code << "\tPSH ";
            
            while (((next >= '0') && (next <= '9')) || (next == '.') || (next == ',')) {
                rpn.get(c);
                code << c;
                next = rpn.peek();
            }
            
            code << std::endl;
        }
        
        else if ((next == 'T') || (next == 'F')) {
            code << "\tPSH ";
            
            rpn.get(c);
            code << c << std::endl;
            next = rpn.peek();
        }
        
        else if (next == '^') {
            rpn.get(c);
            code << "\tPOW" << std::endl;
        }
        
        else if (next == '~') {
            rpn.get(c);
            code << "\tNOT" << std::endl;
        }
        
        else if (next == 'p') {
            rpn.get(c);
            code << "\tPOS" << std::endl;
        }
        
        else if (next == 'm') {
            rpn.get(c);
            code << "\tNEG" << std::endl;
        }
        
        else if (next == '*') {
            rpn.get(c);
            code << "\tMUL" << std::endl;
        }
        
        else if (next == '/') {
            rpn.get(c);
            code << "\tDIV" << std::endl;
        }
        
        else if (next == '&') {
            rpn.get(c);
            code << "\tAND" << std::endl;
        }
        
        else if (next == '+') {
            rpn.get(c);
            code << "\tSUM" << std::endl;
        }
        
        else if (next == '-') {
            rpn.get(c);
            code << "\tSUB" << std::endl;
        }
        
        else if (next == '<') {
            rpn.get(c);
            code << "\tLES" << std::endl;
        }
        
        else if (next == '>') {
            rpn.get(c);
            code << "\tMOR" << std::endl;
        }
        
        else if (next == '=') {
            rpn.get(c);
            code << "\tEQL" << std::endl;
        }
        
        else if (next == '#') {
            rpn.get(c);
            code << "\tNEQ" << std::endl;
        }
        
        else if (next == '!') {
            rpn.get(c);
            code << "\tOR_" << std::endl;
        }
        
        else if (next == 'f') {
            rpn.get(c);
            code << "\tFRC" << std::endl;
        }
        
        else if (next == 'e') {
            rpn.get(c);
            code << "\tENT" << std::endl;
        }
        
        else if (next == ' ')
            rpn.get(c);
        
        else
            ok = 0;
        
        next = rpn.peek();
    }
    
    code << "\tRES\n}";
    
    return ok;
}


int main(int argc, const char *argv[]) {
    std::stringstream rpn;
    char code_path[1024], wrong_symbol = '\0';
    u_long wrong_symbol_place = 0, wrong_symbol_line = 0;
    uint8_t recognizer_res = OK;
    bool from_file = 0, ok = 0;
    
    if ((argc > 3) || (argc == 2)) {
        std::cerr << std::endl << "Некорректное число входных аргументов" << std::endl;
    }
    
    else if (argc == 1) {
        std::ofstream code;
        
        ok = 1;
        from_file = 0;
        
        std::cout << std::endl << "compiler> ";
        
        recognizer_res = recognizer(std::cin, from_file, wrong_symbol, wrong_symbol_place, wrong_symbol_line, rpn);
        
        if (recognizer_res == OK) {
            std::cout << rpn.str() << std::endl;
            
            ok = 0;
            
            while (!ok) {
                std::cout << std::endl << "Введите имя файла для записи кода для стекового устройства" << std::endl;
                rewind(stdin);
                std::cin.getline(code_path, 1024, '\n');
                
                code.open(code_path, std::ios::trunc);
                
                ok = code.is_open();
                
                if (ok) {
                    ok = generate_code(rpn, code);
                    code.close();
                }
                
                else
                    std::cerr << std::endl << "Ошибка при открытии файла «" << code_path << "»" << std::endl;
            }
        }
    }
    
    else if (argc == 3) {
        std::ifstream fin;
        std::ofstream fout;
        
        fin.open(argv[1]);
        fout.open(argv[2], std::ios_base::trunc);
        
        if (!fin.is_open())
            std::cerr << "Не удалось открыть файл «" << argv[1] << "»" << std::endl;
            
        else if (!fout.is_open())
            std::cerr << "Не удалось создать файл «" << argv[2] << "»" << std::endl;
        
        else if (fin.peek() == EOF)
            std::cerr << "Файл «" << argv[1] << "» пуст" << std::endl;
        
        else {
            code_path[0] = '\0';
            strcat(code_path, argv[2]);
            from_file = 1;
            recognizer_res = recognizer(fin, from_file, wrong_symbol, wrong_symbol_place, wrong_symbol_line, rpn);
            
            if (recognizer_res == OK) {
                fout << rpn.str() << std::endl << std::endl;
                ok = generate_code(rpn, fout);
            }
            
            fin.close();
            fout.close();
        }
    }
    
    if (recognizer_res == OK) {
        if (ok)
            std::cout << std::endl << "Код для САЛУ записан в файл «" << code_path << "»" << std::endl;
        
        else
            std::cout << std::endl << "Ошибка при записи кода для САЛУ в файл «" << code_path << "»" << std::endl;
        }
        
        else {
            if (recognizer_res & MEM_ERR)
                std::cerr << "Ошибка памяти" << std::endl;
            
            if (recognizer_res & READ_ERR)
                std::cerr << "Ошибка чтения" << std::endl;
            
            if (recognizer_res & UNEXPECTED) {
                std::cerr << "Неожиданный символ ";
                
                if (wrong_symbol == '\n')
                    std::cerr << "конца строки";
                
                else if (wrong_symbol == EOF)
                    std::cerr << "конца файла";
                
                else
                    std::cerr << "«" << wrong_symbol << "» (#" << wrong_symbol_place << ')';
                
                if (from_file) {
                    std::cerr << " в";
                    if (wrong_symbol_line == 2)
                        std::cerr << 'o';
                    std::cerr << ' ' << wrong_symbol_line << " строке";
                }
                
                std::cerr << std::endl;
            }
            
            if (recognizer_res & BRACKETS)
                std::cerr << "Несоответствие между количествами открывающих и закрывающих скобок" << std::endl;
        }
        
    std::cout << std::endl;
    
    return 0;
}
