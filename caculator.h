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
    void printline(std::ostream &os);
    calulation(int ileft, char iop, int iright);
    calulation(calulation ileft, char iop, calulation iright);
    ~calulation();
    calulation();
    long double result;
    long double getleftValue();
    long double getrightValue();
    void random_left();
    void random_right();
    

private:
    char op;
    void * left;
    bool leftIsCalculation;
    void * right;
    bool rightIsCalculation;
    
};

#include "caculation.cpp"