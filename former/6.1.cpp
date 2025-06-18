
//Project - Dice
#include <iostream>
#include <cstdlib>
using namespace std;
class Dice
{
public:
    Dice() : iSides(6), rollCounts(0), sideCounts{0, 0, 0, 0, 0, 0}, last(0) {
    };
    int rollDice()
    {
        last = roll();
        sideCounts[last - 1]++;
        rollCounts++;
        return last;
    }
    int rollCount()
    {
        return rollCounts;
    }
    int roll()
    {
        return rand() % iSides + 1; // Use iSides for flexibility
    }
    int sideCount(int side) {
        if (side >= 1 && side <= iSides) {
            return sideCounts[side - 1];
        } else {
            return -1; // Indicate an invalid side
        }
    }
    int getSides() {
        return iSides;
    }
    int iSides;
private:
    int rollCounts;
    int sideCounts[6];
    int last;
};