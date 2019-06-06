#include "globals.hpp"
#include "compiler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


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
