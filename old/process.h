//process.h
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
extern std::string pass;
extern std::stringstream temps;
extern int mems;
extern int input;

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
