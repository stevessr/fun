#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <sstream>
#include "caculator.h"
calulation c;
short mode = 0;
short op_type = 0;
string temp = "";
int mems;
int consoleWidth, consoleHeight;
int input = 0;

using namespace std;

void introduction()
{
    c.printline(cout);
    cout << setw(consoleWidth - 1) << left << "*欢迎来到小学运算练习器" << "*" << endl
         << setw(consoleWidth - 1) << left << "*本程序由C++驱动" << "*" << endl
         << setw(consoleWidth - 1) << left << "*功能：可以进行加、减、乘、除运算练习、四则混合运算" << "*" << endl
         << setw(consoleWidth - 1) << left << "*练习方法：游戏型、普通练习型" << "*" << endl;
    c.printline(cout);
}

void mode_get()
{
    cout << setw(consoleWidth - 1) << left << "*请选择练习模式：" << "*" << endl
         << setw(consoleWidth - 1) << left << "*1.游戏型" << "*" << endl
         << setw(consoleWidth - 1) << left << "*2.普通练习型" << "*" << endl;
    cout << "*你的选择是(输入0退出游戏)：";
    cin >> temp;
    while (temp != "1" && temp != "2" && temp != "0")
    {
        cout << "*输入错误，请重新输入：";
        cin >> temp;
    }
    if (temp == "1")
    {
        mode = 1;
        cout << setw(consoleWidth - 1) << left << "*游戏模式已开启" << "*" << endl;
    }
    else if (temp == "2")
    {
        mode = 0;
        cout << setw(consoleWidth - 1) << left << "*普通练习模式已开启" << "*" << endl;
    }
    else
    {
        c.printline(cout);
        mode = -1;
    }
    c.printline(cout);
}

void game()
{
    cout << setw(consoleWidth - 1) << left << "*你已进入游戏模式" << "*" << endl;
    double towait;
    cout << "*请输入每道题的限时（单位:秒）：";
    auto start = std::chrono::high_resolution_clock::now();
    cin >> towait;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << setw(consoleWidth - 1) << left << "*无尽闯关即将开始" << "*" << endl;
    cout << "*";
    system("pause");
    c.printline(cout);
    srand(static_cast<unsigned int>(duration) % 65535);
    long long a = 0;
    while (true)
    {
        mems = 1;
        c.random();
        cout << "*";
        cout << c;
        cout << "=" << endl;
        cout << "*请输入答案(取整数部分)：";
        start = std::chrono::high_resolution_clock::now();
        cin >> temp;
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << setw(consoleWidth - 1) << left << "*耗时: " + to_string(duration) + "毫秒" << "*" << endl;
        temp >> input;
        if (input == (c.result))
        {
            cout << setw(consoleWidth - 1) << left << "*答案正确" << "*" << endl;
            cout << "*";
            system("pause");
            system("cls");
        }
        else
        {
            cout << setw(consoleWidth - 1) << left << "*答案错误，游戏终止，正确答案" + to_string(int(c.result)) << "*" << endl;
            break;
        }
        if (duration > 1000 * towait)
        {
            cout << setw(consoleWidth - 1) << left << "*超时，游戏终止" << "*" << endl;
            break;
        }
        c.printline(cout);
        a++;
    }
    cout << setw(consoleWidth - 1) << left << "*累计成功" + to_string(a) + "次" << "*" << endl;
}

void pratise()
{
    cout << "*请选择你想做的题目类型（1、加法，2、减法，3、乘法，4、除法，5、混合，q、退出）：";
    cin >> temp;
    while (temp != "5" && temp != "4" && temp != "3" && temp != "2" && temp != "1" && temp != "q")
    {
        cout << "*输入错误，请重新输入:" << endl;
        cin >> temp;
    }
    if (temp == "q")
    {
        mode = -1;
        return;
    }
    long long towait, tars;
    cout << "*你想做几题：";
    auto start = std::chrono::high_resolution_clock::now();
    cin >> towait;
    auto end = chrono::high_resolution_clock::now();
    tars = towait;
    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << "*";
    system("pause");
    c.printline(cout);
    srand(static_cast<unsigned int>(duration) % 65535);
    long long a = 0;
    while (towait--)
    {
        c.printline(cout);
        c.random(temp);
        cout << "*";
        cout << c;
        cout << "=" << endl;
        cout << "*请输入答案(取整数部分,输入q退出)：";
        start = std::chrono::high_resolution_clock::now();
        cin >> temp;
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << setw(consoleWidth - 1) << left << "*耗时: " + to_string(duration) + "毫秒" << "*" << endl;
        temp >> input;
        if (input == int(c.result))
        {
            cout << setw(consoleWidth - 1) << left << "*答案正确" << "*" << endl;
            a++;
        }
        else if (temp == "q")
        {
            mode = -1;
            break;
        }
        else
        {
            cout << setw(consoleWidth - 1) << left << "*答案错误，正确答案" + to_string(int(c.result)) << "*" << endl;
        }
    }
    c.printline(cout);
    cout << setw(consoleWidth - 1) << left << "*累计成功" + to_string(a) + "次" << "*" << endl;
    cout << setw(consoleWidth - 1) << left << "*成功率" + to_string(a * 1.0 / tars) + "%" << "*" << endl;
}

void action()
{
    system("cls");
    c.printline(cout);
    while (mode != -1)
    {
        switch (mode)
        {
        case 1:
            game();
            break;
        case 0:
            pratise();
            break;
        case -1:
            break;
        default:
            break;
        }
    }
    if (mode == -1){
        
    }
        cout << "*";
}

void thanks(){
    cout << setw(consoleWidth - 1) << left << "*感谢你的使用" << "*" << endl;
}