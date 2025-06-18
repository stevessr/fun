#include <iostream>
#include <string>
#include <assert.h>
using namespace std;

const short days[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                    {30, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
class Date
{
public:
    Date(int y = 0, int m = 1, int d = 1) : year(y), month(m), day(d) {}
    string toText()
    {
        return to_string(year) + "-" + to_string(month) + "-" + to_string(day);
    }
    Date prevDay()
    {
        if (day > 1)
        {
            return Date(year, month, day - 1);
        }
        else if (month > 1)
        {
            return Date(year, month - 1, days[year % 4 == 0 && ((year & 100 == 0) ? (year % 400) : true)][month - 2]);
        }
        else
        {
            return (year - 1, 12, 31);
        }
    }
    Date nextDay(){
        if (day < days[year % 4 == 0 && ((year & 100 == 0) ? (year % 400) : true)][month - 2])
        {
            return Date(year, month, day + 1 );
        }
        else if (month < 12)
        {
            return Date(year, month + 1, 1);
        }
        else
        {
            return (year + 1, 1, 1);
        }
    }

    int year, month, day;
};