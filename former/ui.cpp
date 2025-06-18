//ui.cpp
#pragma once
#include <iostream>
#include <string>
#include <iomanip>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#endif

int consoleWidth, consoleHeight;
std::ostream &os = std::cout;
bool uninit = true;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// 获取当前控制台的宽度和高度 windows only
void init_windows()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(console, &bufferInfo);
    consoleWidth = bufferInfo.dwSize.X;
    consoleHeight = bufferInfo.dwSize.Y;
    uninit = false;
}

#else
void init()
{
    consoleHeight = 128;
    consoleWidth = 46;
}
#endif

void printline()
{
    if (uninit)
    {
        init_windows();
    }
    for (int i = 0; i < consoleWidth; i++)
    {
        os << "*";
    }
    os << std::endl;
}

void clear_screen()
{
    system("cls");
}
void after_finish()
{
    os << "*";
    system("pause");
    clear_screen();
}
void waiting()
{
    os << "*";
    system("pause");
}

void print_line(std::string str)
{
    if (uninit)
    {
        init_windows();
    }
    os << std::setw(consoleWidth - 1) << std::left << "*" + str << "*" << std::endl;
}
void input_line(std::string str)
{
    if (uninit)
    {
        init_windows();
    }
    os << std::setw(consoleWidth - 1) << std::left << "*" + str << "*";
    for (int i = (consoleWidth - 2); i > str.length(); i--)
    {
        os.put('\b');
    }
}