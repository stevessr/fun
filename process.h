#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <string>
#include <vector>

#include "caculator.h"

extern calulation c; 
extern short mode;
extern std::string temp;
extern std::string save;
extern int mems;
extern int input;
extern std::stringstream temps;
extern short pass;

// 函数声明

void clear_screen();
void after_finish();
void waiting();
void print_line(std::string str);
void input_line(std::string str);
void introduction();
void mode_get();
void game();
void pratise();
void action();
void thanks();

#include "process.cpp"

#endif // PROCESS_H
