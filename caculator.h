//caculator.h
#pragma once

#include <ostream>
#include <string>

class calulation
{
public:
    friend auto operator<<(std::ostream &os, const calulation &c)
    {
        c.output(os,0);
    }
    void output(std::ostream &os) const;
    void output(std::ostream &os, int parentPriority) const;
    bool random();
    bool random(std::string oper);
    bool random(std::string oper,short mode);
    bool random(long difficulty);
    bool random(int max);
    bool random(short mode);
    bool random(std::string oper,int max);
    calulation(int ileft, char iop, int iright);
    calulation(calulation ileft, char iop, calulation iright);
    ~calulation();
    calulation();
    long double result;
    long double getleftValue();
    long double getrightValue();
    void random_left();
    void random_left(int current_chance, int max_value);
    void random_right();
    void random_right(int current_chance, int max_value);
    int get_result();
    int get_difficulty_params_for_child(int current_chance,int max_value);
    std::string get_string() const;
    

private:
    char op;
    void * left;
    bool leftIsCalculation;
    void * right;
    bool rightIsCalculation;
    
};

#include "caculation.cpp"