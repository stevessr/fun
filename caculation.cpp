#pragma once
#include "caculator.h"
#include <string>
#include <random>
#include <ctime>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#endif

const short chance = 6;
const short max_member = 10;
extern int consoleWidth, consoleHeight;

extern int mems;

auto safe_delete = [](void *ptr, bool is_calculation)
{
    if (!ptr)
        return;
    is_calculation ? delete static_cast<calulation *>(ptr)
                   : delete static_cast<int *>(ptr);
};

bool calulation::random()
{
    random_left();
    random_right();
    switch (rand() % 4)
    {
    case 0:
        op = '+';
        result = getleftValue() + getrightValue();
        break;
    case 1:
        op = '-';
        result = getleftValue() - getrightValue();
        break;
    case 2:
        op = '*';
        result = getleftValue() * getrightValue();
        break;
    case 3:
        op = '/';
        result = getleftValue() / getrightValue();
        break;
    default:
        op = '+';
        result = getleftValue() + getrightValue();
        break;
    }
    if (result > 1000 || result < -1000)
    {
        random();
    }
    return true;
}

bool calulation::random(std::string oper)
{

    safe_delete(left, leftIsCalculation);
    safe_delete(right, rightIsCalculation);
    rightIsCalculation = false;
    right = new int(rand() % 1000);
    leftIsCalculation = false;
    left = new int(rand() % 1000);
    if (oper == "+")
    {
        op = '+';
        result = getleftValue() + getrightValue();
    }
    else if (oper == "-")
    {
        op = '-';
        result = getleftValue() - getrightValue();
    }
    else if (oper == "*")
    {
        op = '*';
        result = getleftValue() * getrightValue();
    }
    else if (oper == "/")
    {

        op = '/';
        result = getleftValue() / getrightValue();
    }
    return true;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// 获取当前控制台的宽度和高度 windows only
void calulation::printline(std::ostream &os)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(console, &bufferInfo);
    consoleWidth = bufferInfo.dwSize.X;
    consoleHeight = bufferInfo.dwSize.Y;
    for (int i = 0; i < consoleWidth; i++)
    {
        os << "*";
    }
    os << std::endl;
}
#else
void calulation::printline(std::ostream &os)
{
    consoleWidth = 40;
    for (int i = 0; i < 20; i++)
    {
        os << "*";
    }
    os << std::endl;
}
#endif

int getOperatorPriority(char op)
{
    switch (op)
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    // 可以根据需要添加更多的运算符和优先级
    default:
        return 0;
    }
}
void calulation::output(std::ostream &os) const
{
    if (leftIsCalculation)
    {
        os << "(";
        ((calulation *)left)->output(os);
        os << ")";
    }
    else
    {
        os << *(int *)left;
    }
    os << op;
    if (rightIsCalculation)
    {
        os << "(";
        ((calulation *)right)->output(os);
        os << ")";
    }
    else
    {
        os << *(int *)right;
    }
}

void calulation::output(std::ostream &os, int parentPriority) const
{
    int currentPriority = getOperatorPriority(op);

    if (leftIsCalculation)
    {
        if (currentPriority < parentPriority)
        {
            os << "(";
        }
        ((calulation *)left)->output(os, currentPriority);
        if (currentPriority < parentPriority)
        {
            os << ")";
        }
    }
    else
    {
        os << *(int *)left;
    }
    os << op;

    if (rightIsCalculation)
    {
        if (currentPriority <= parentPriority)
        {
            os << "(";
        }
        ((calulation *)right)->output(os, currentPriority);
        if (currentPriority <= parentPriority)
        {
            os << ")";
        }
    }
    else
    {
        os << *(int *)right;
    }
}

calulation::~calulation()
{
    safe_delete(left, leftIsCalculation);
    safe_delete(right, rightIsCalculation);

    left = nullptr;
    right = nullptr;
}

calulation::calulation()
{
    left = nullptr;
    right = nullptr;
    leftIsCalculation = false;
    rightIsCalculation = false;
}

long double calulation::getleftValue()
{
    if (leftIsCalculation)
    {
        return ((calulation *)left)->result;
    }
    else
    {
        return *((int *)left);
    }
}

long double calulation::getrightValue()
{
    if (rightIsCalculation)
    {
        return ((calulation *)right)->result;
    }
    else
    {
        return *((int *)right);
    }
}

void calulation::random_left()
{
    safe_delete(left, leftIsCalculation);
    if (rand() % chance == 0 && mems < max_member)
    {
        leftIsCalculation = true;
        left = new calulation();
        ((calulation *)left)->random();
    }
    else
    {
        leftIsCalculation = false;
        left = new int(rand() % 1000);
    }
}

void calulation::random_right()
{
    safe_delete(right, rightIsCalculation);
    if (rand() % chance == 0 && mems < max_member)
    {
        rightIsCalculation = true;
        right = new calulation();
        ((calulation *)right)->random();
    }
    else
    {
        rightIsCalculation = false;
        right = new int(rand() % 1000);
    }
}