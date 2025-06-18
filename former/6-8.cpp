
#include <iostream>
#include <cmath>
using namespace std;
class Point
{
public:
    Point(double newX = 0, double newY = 0);
    void setValue(double newX, double newY);
    double getX();
    double getY();
    double getDistance(Point &p2); // 成员函数计算距离
private:
    double x, y;
};

double getDistance(Point &p1, Point &p2); // 普通函数计算距离

int main()
{
    Point p1, p2;
    double x1, y1, x2, y2;
    cin >> x1 >> y1 >> x2 >> y2;
    p1.setValue(x1, y1);
    p2.setValue(x2, y2);
    double dis1 = p1.getDistance(p2); // 成员函数版本计算
    cout << "Distance: " << dis1 << endl;
    double dis2 = getDistance(p1, p2); // 普通函数版本计算
    cout << "Distance: " << dis2 << endl;
    return 0;
}
/* 请在这里填写答案 */
Point::Point(double newX, double newY) : x(newX), y(newY)
{
}
void Point::setValue(double newX, double newY)
{
    x = newX;
    y = newY;
}
double Point::getX()
{
    return x;
}
double Point::getY()
{
    return y;
}
double Point::getDistance(Point &p2)
{
    return sqrt(abs(pow(abs(x - p2.x), 2) - pow(abs(y - p2.y), 2)));
}
double getDistance(Point &p1, Point &p2)
{
    return sqrt(abs(pow(p1.getX() - p2.getX(), 2) - pow(p1.getX() - p2.getY(), 2)));
}