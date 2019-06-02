uint8_t recognizer(std::istream &in, bool from_file, u_long &curr, u_long &curr_line, std::stringstream &rpn) {
    bool ok = 1, prev_lexeme_is_num = 0, prev_lexeme_is_bool = 0, add_space = 0, o = 0;
    char next, c, num[16], o_str[2];
    uint8_t state = OK;
    Stack<char> operators;
    
    curr = 0;
    curr_line = 1;
    o_str[1] = '\0';
    
    next = in.peek();
    
    while ((state == OK) && (((from_file) && (in.peek() != EOF)) || ((!from_file) && (in.peek() != '\n')))) {
        if (next == ' ') {
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
                prev_lexeme_is_bool = 0;
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
            
            rpn << c;
            next = in.peek();
            
            if ((next == '0') || (next == '1')) {
                in.get(c);
                curr++;
                rpn << c;
            }
            
            else if (((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z')))
                while (((next >= '0') && (next <= '9')) || ((next >= 'A') && (next <= 'Z')) || ((next >= 'a') && (next <= 'z'))) {
                    in.get(c);
                    curr++;
                    rpn << c;
                    next = in.peek();
                }
            
            else
                state |= UNEXPECTED;
            
            prev_lexeme_is_num = 0;
            prev_lexeme_is_bool = 1;
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
            prev_lexeme_is_bool = 0;
            add_space = 1;
        }
        
        else if (next == '+') {
            in.get(c);
            curr++;
            
            if (prev_lexeme_is_num)
                prev_lexeme_is_num = 0;
            
            else if (prev_lexeme_is_bool)
                state |= UNEXPECTED;
            
            else
                c = 'p';
            
            o = 1;
        }
        
        else if (next == '-') {
            in.get(c);
            curr++;
            
            if (prev_lexeme_is_num)
                prev_lexeme_is_num = 0;
            
            else if (prev_lexeme_is_bool)
                state |= UNEXPECTED;
            
            else
                c = 'm';
            
            o = 1;
        }
        
        else if (next == '*') {
            in.get(c);
            curr++;
            
            if (prev_lexeme_is_num) {
                o = 1;
                prev_lexeme_is_num = 0;
            }
            
            else
                state |= UNEXPECTED;
        }
        
        else if (next == '/') {
            in.get(c);
            curr++;
            
            if (prev_lexeme_is_num) {
                o = 1;
                prev_lexeme_is_num = 0;
            }
            
            else
                state |= UNEXPECTED;
        }
        
        else if (next == '^') {
            in.get(c);
            curr++;
            
            if (prev_lexeme_is_num) {
                o = 1;
                prev_lexeme_is_num = 0;
            }
            
            else
                state |= UNEXPECTED;
        }
        
        else if (next == '~') {
            in.get(c);
            curr++;
            
            if ((prev_lexeme_is_num) || (prev_lexeme_is_bool))
                state |= UNEXPECTED;
            
            else
                o = 1;
        }
        
        else if ((next == '&') || (next == '!')) {
            in.get(c);
            curr++;
            
            if (prev_lexeme_is_bool) {
                o = 1;
                prev_lexeme_is_bool = 0;
                prev_lexeme_is_num = 0;
            }
            
            else
                state |= UNEXPECTED;
        }
        
        else if ((next == '<') || (next == '>') || (next == '=') || (next == '#')) {
            in.get(c);
            curr++;
            
            if (prev_lexeme_is_num) {
                o = 1;
                prev_lexeme_is_num = 0;
            }
            
            else
                state |= UNEXPECTED;
        }
        
        else if (next == '(') {
            in.get(c);
            curr++;
            
            operators.push(c);
            
            prev_lexeme_is_num = 0;
            prev_lexeme_is_bool = 0;
        }
        
        else if (next == ')') {
            in.get(c);
            curr++;
            
            while ((!operators.isEmpty()) && (operators.peek() != '(')) {
                o_str[0] = operators.pop();
                rpn << o_str;
                add_space = 0;
            }
            
            if (operators.isEmpty())
                state |= BRACKETS;
            else
                operators.del();
            
            prev_lexeme_is_num = 1;
            prev_lexeme_is_bool = 1;
        }
        
        else
            state |= UNEXPECTED;
        
        while ((state == OK) && (o)) {
            if ((operators.isEmpty()) || (operators.peek() == '(')) {
                operators.push(c);
                o = 0;
            }
            
            else if ((!operators.isEmpty()) && (priority(c) > priority(operators.peek()))) {
                operators.push(c);
                o = 0;
            }
            
            else if ((!operators.isEmpty()) && (priority(c) == priority(operators.peek())) && (!associativity(c))) {
                operators.push(c);
                o = 0;
            }
            
            else {
                o_str[0] = operators.pop();
                rpn << o_str;
                add_space = 0;
            }
        }
        
        next = in.peek();
    }
    
    if (state == OK)
        while ((!operators.isEmpty()) && (state == OK)) {
            o_str[0] = operators.pop();
            
            if (o_str[0] == '(')
                state |= BRACKETS;
            
            else
                rpn << o_str;
        }
    
    if (state & UNEXPECTED)
        curr++;
    
    return state;
}