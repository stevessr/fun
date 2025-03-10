// caculation.cpp
#pragma once
#include "caculator.h"
#include <string>
#include <random>
#include <iostream>

const short chance = 6;
const short max_member = 10;
extern int consoleWidth, consoleHeight;
const int max_target = 1000;

extern int mems;

// Independent Difficulty Control Function
void get_difficulty_params(int difficulty, int &current_chance, int &max_value)
{
    if (difficulty < 1 || difficulty > 5)
    {
        std::cerr << "Invalid difficulty level. Setting to default (3)." << std::endl;
        difficulty = 3; // Default difficulty
    }

    switch (difficulty)
    {
    case 1:
        current_chance = 1;
        max_value = 100;
        break;
    case 2:
        current_chance = 3;
        max_value = 500;
        break;
    case 3:
        current_chance = chance;
        max_value = 1000;
        break;
    case 4:
        current_chance = chance + 2;
        max_value = 2000;
        break;
    case 5:
        current_chance = chance + 4;
        max_value = 3000;
        break;
    default:
        current_chance = chance;
        max_value = 1000;
        break;
    }
}

auto safe_delete = [](void *ptr, bool is_calculation)
{
    if (!ptr)
        return;
    is_calculation ? delete static_cast<calulation *>(ptr)
                   : delete static_cast<int *>(ptr);
};

auto get_custom_string = [](void *ptr, bool is_calculation) -> std::string
{
    if (!ptr)
        return "";
    return is_calculation ? static_cast<calulation *>(ptr)->get_string()
                          : std::to_string(*static_cast<int *>(ptr));
};

// New random Function
bool calulation::random(long difficulty)
{
    int current_chance, max_value;
    get_difficulty_params(difficulty, current_chance, max_value);

    // Use the difficulty-specific parameters in random_left and random_right
    random_left(current_chance, max_value);
    random_right(current_chance, max_value);

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
        while (getleftValue() == 0 || getrightValue() == 0 || int(getleftValue()) % int(getrightValue()) != 0)
        {
            random_left(current_chance, max_value);
            random_right(current_chance, max_value);
        }
        result = getleftValue() / getrightValue();
        break;
    default:
        op = '+';
        result = getleftValue() + getrightValue();
        break;
    }
    if (result > max_target || result < -max_target)
    {
        random(difficulty);
    }
    return true;
}

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
        while (getleftValue() == 0 || getrightValue() == 0 || int(getleftValue()) % int(getrightValue()) != 0)
        {
            random_left();
            random_right();
        }
        result = getleftValue() / getrightValue();
        break;
    default:
        op = '+';
        result = getleftValue() + getrightValue();
        break;
    }
    if (result > max_target || result < -max_target)
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
    if (oper == "1")
    {
        op = '+';
        result = getleftValue() + getrightValue();
    }
    else if (oper == "2")
    {
        op = '-';
        result = getleftValue() - getrightValue();
    }
    else if (oper == "3")
    {
        op = '*';
        result = getleftValue() * getrightValue();
    }
    else if (oper == "4")
    {

        op = '/';
        while (getleftValue() == 0 || getrightValue() == 0 || int(getleftValue()) % int(getrightValue()) != 0)
        {
            right = new int(rand() % 1000);
            left = new int(rand() % 1000);
        }
        result = getleftValue() / getrightValue();
    }
    else
    {
        random();
    }
    return true;
}

bool calulation::random(std::string oper, short mode)
{

    safe_delete(left, leftIsCalculation);
    safe_delete(right, rightIsCalculation);
    rightIsCalculation = false;
    right = new int(rand() % 1000);
    leftIsCalculation = false;
    left = new int(rand() % 1000);
    if (oper == "1")
    {
        op = '+';
        result = getleftValue() + getrightValue();
    }
    else if (oper == "2")
    {
        op = '-';
        result = getleftValue() - getrightValue();
    }
    else if (oper == "3")
    {
        op = '*';
        result = getleftValue() * getrightValue();
    }
    else if (oper == "4")
    {

        op = '/';
        while (getleftValue() == 0 || getrightValue() == 0 || int(getleftValue()) % int(getrightValue()) != 0)
        {
            right = new int(rand() % 1000);
            left = new int(rand() % 1000);
        }
        result = getleftValue() / getrightValue();
    }
    else
    {
        random(mode);
    }
    return true;
}

bool calulation::random(short mode)
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
        while (getleftValue() == 0 || getrightValue() == 0 || int(getleftValue()) % int(getrightValue()) != 0)
        {
            random_left();
            random_right();
        }
        result = getleftValue() / getrightValue();
        break;
    default:
        op = '+';
        result = getleftValue() + getrightValue();
        break;
    }
    if (result > max_target || result < -max_target)
    {
        random();
    }
    return true;
}

bool calulation::random(int max)
{
    do
    {
        random();
    } while (result > max || result < -max);
    return true;
}

bool calulation::random(std::string oper, int max)
{
    do
    {
        random(oper);
    } while (result > max || result < -max);
    return true;
}

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
        if (get_op() != ((calulation *)left)->get_op())
        {
            os << "(";
        }
        ((calulation *)left)->output(os, currentPriority);
        if (get_op() != ((calulation *)left)->get_op())
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
        if (get_op() != ((calulation *)right)->get_op())
        {
            os << "(";
        }
        ((calulation *)right)->output(os, currentPriority);
        if (get_op() != ((calulation *)right)->get_op())
        {
            os << ")";
        }
    }
    else
    {
        os << *(int *)right;
    }
}

char calulation::get_op() const
{
    return op;
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
        return static_cast<long double>(*((int *)left));
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
        return static_cast<long double>(*((int *)right));
    }
}
// new random_left Function
void calulation::random_left(int current_chance, int max_value)
{
    safe_delete(left, leftIsCalculation);
    if (rand() % current_chance == 0 && mems < max_member)
    {
        leftIsCalculation = true;
        left = new calulation();
        ((calulation *)left)->random(get_difficulty_params_for_child(current_chance, max_value));
        mems++;
    }
    else
    {
        leftIsCalculation = false;
        left = new int(rand() % max_value);
    }
}
// original random_left Function
void calulation::random_left()
{
    safe_delete(left, leftIsCalculation);
    if (rand() % chance == 0 && mems < max_member)
    {
        leftIsCalculation = true;
        left = new calulation();
        ((calulation *)left)->random();
        mems++;
    }
    else
    {
        leftIsCalculation = false;
        left = new int(rand() % 1000);
    }
}
// new random_right Function
void calulation::random_right(int current_chance, int max_value)
{
    safe_delete(right, rightIsCalculation);
    if (rand() % current_chance == 0 && mems < max_member)
    {
        rightIsCalculation = true;
        right = new calulation();
        ((calulation *)right)->random(get_difficulty_params_for_child(current_chance, max_value));
        mems++;
    }
    else
    {
        rightIsCalculation = false;
        right = new int(rand() % max_value);
    }
}
// original random_right Function
void calulation::random_right()
{
    safe_delete(right, rightIsCalculation);
    if (rand() % chance == 0 && mems < max_member)
    {
        rightIsCalculation = true;
        right = new calulation();
        ((calulation *)right)->random();
        mems++;
    }
    else
    {
        rightIsCalculation = false;
        right = new int(rand() % 1000);
    }
}
// new function
int calulation::get_difficulty_params_for_child(int current_chance, int max_value)
{
    int temp_current_chance, temp_max_value;
    for (int i = 1; i <= 5; i++)
    {
        get_difficulty_params(i, temp_current_chance, temp_max_value);
        if (current_chance == temp_current_chance && max_value == temp_max_value)
        {
            return i;
        }
    }
    return 3;
}

int calulation::get_result()
{
    return static_cast<int>(result);
}

std::string calulation::get_string() const
{
    return get_custom_string(left, leftIsCalculation) + op + get_custom_string(right, rightIsCalculation);
}
