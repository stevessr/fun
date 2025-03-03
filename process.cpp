#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <sstream>
#include "caculator.h"

using namespace std;

calulation c;
short mode = 0;
short op_type = 0;
string temp = "";
string save = "";
int mems;
int consoleWidth, consoleHeight;
int input = 0;
stringstream temps;
short pass = 0;

void after_finish()
{
    cout << "*";
    system("pause");
    system("cls");
}
void waiting()
{
    cout << "*";
    system("pause");
}

void print_line(string str)
{
    cout << setw(consoleWidth - 1) << left << "*" + str << "*" << endl;
}
void introduction()
{
    c.printline(cout);
    cout << setw(consoleWidth - 1) << left << "*欢迎来到小学运算练习器" << "*" << endl
         << setw(consoleWidth - 1) << left << "*本程序由C++驱动" << "*" << endl
         << setw(consoleWidth - 1) << left << "*功能：可以进行加、减、乘、除运算练习、四则混合运算" << "*" << endl
         << setw(consoleWidth - 1) << left << "*练习方法：游戏型、普通练习型" << "*" << endl;
}

void mode_get()
{
    c.printline(cout);
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
    pass = 0;
    while (pass != 1)
    {
        cout << "*你确定嘛?(输入1确定,2反悔)";
        cin >> pass;
        while (pass == 2)
        {
            cout << "*你的选择是(输入0退出游戏)：";
            cin >> temp;
            while (temp != "1" && temp != "2" && temp != "0")
            {
                cout << "*输入错误，请重新输入：";
                cin >> temp;
            }
            cout << "*你确定嘛?(输入1确定,2反悔)";
            cin >> pass;
        }
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
        mode = -1;
    }
    c.printline(cout);
}

void game()
{
    cout << setw(consoleWidth - 1) << left << "*你已进入游戏模式" << "*" << endl;
    double towait = 0;
    cout << "*请输入每道题的限时（单位:秒）：";
    auto start = std::chrono::high_resolution_clock::now();
    cin >> towait;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    pass = 0;
    while (pass != 1)
    {
        cout << "*你确定嘛?(输入1确定,2反悔)";
        cin >> pass;
        while (pass == 2)
        {
            cout << "*请输入每道题的限时（单位:秒）：";
            cin >> towait;
            cout << "*你确定嘛?(输入1确定,2反悔)";
            cin >> pass;
        }
    }
    while (towait == 0)
    {
        cout << "*输入有误，请重新输入：";
        cin >> towait;
    }
    cout << setw(consoleWidth - 1) << left << "*无尽闯关即将开始" << "*" << endl;
    waiting();
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
        temps = stringstream(temp);
        temps >> input;
        if (input == c.get_result())
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
    after_finish();
}

void pratise()
{
    cout << "*请选择你想做的题目类型（1、加法，2、减法，3、乘法，4、除法，5、混合，q、退出）：";
    cin >> temp;
    pass = 0;
    while (pass != 1)
    {
        cout << "*你确定嘛?(输入1确定,2反悔)";
        cin >> pass;
        while (pass == 2)
        {
            cout << "*请选择你想做的题目类型（1、加法，2、减法，3、乘法，4、除法，5、混合，q、退出）：";
            cin >> temp;
            while (temp != "5" && temp != "4" && temp != "3" && temp != "2" && temp != "1" && temp != "q")
            {
                cout << "*输入错误，请重新输入:" << endl;
                cin >> temp;
            }
            cout << "*你确定嘛?(输入1确定,2反悔)";
            cin >> pass;
        }
    }
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
    long long towait;
    long tars;
    cout << "*你想做几题：";
    auto start = std::chrono::high_resolution_clock::now();
    cin >> towait;
    auto end = chrono::high_resolution_clock::now();
    pass = 0;
    while (pass != 1)
    {
        cout << "*你确定嘛?(输入1确定,2反悔)";
        cin >> pass;
        while (pass == 2)
        {
            cout << "*你想做几题：";
            cin >> towait;
            cout << "*你确定嘛?(输入1确定,2反悔)";
            cin >> pass;
        }
    }
    tars = towait;
    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    waiting();
    c.printline(cout);
    srand(static_cast<unsigned int>(duration) % 65535);
    long a = 0;
    save = temp;
    vector<string> wrong;
    wrong.empty();
    while (towait--)
    {
        c.printline(cout);
        c.random(save);
        cout << "*";
        cout << c;
        cout << "=" << endl;
        cout << "*请输入答案(取整数部分,输入q退出)：";
        start = std::chrono::high_resolution_clock::now();
        cin >> temp;
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << setw(consoleWidth - 1) << left << "*耗时: " + to_string(duration) + "毫秒" << "*" << endl;
        temps = stringstream(temp);
        temps >> input;
        if (temp == "q")
        {
            mode = -1;
            break;
        }
        else if (input == c.get_result())
        {

            cout << setw(consoleWidth - 1) << left << "*答案正确" << "*" << endl;
            a++;
        }
        else
        {
            cout << setw(consoleWidth - 1) << left << "*答案错误，正确答案" + to_string(int(c.result)) << "*" << endl;
            wrong.push_back(c.get_string());
        }
    }
    c.printline(cout);
    cout << setw(consoleWidth - 1) << left << "*累计成功" + to_string(a) + "次" << "*" << endl;
    cout << setw(consoleWidth - 1) << left << "*成功率" + to_string(a * 1.0 / tars) + "%" << "*" << endl;
    cout << setw(consoleWidth - 1) << left << "总共" + to_string(tars) + "题" << "*" << endl;
    c.printline(cout);
    print_line("以下是错题");
    for(auto &i:wrong){
        print_line(i);
    }
    after_finish();
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
            mode_get();
            break;
        case 0:
            pratise();
            mode_get();
            break;
        case -1:
            break;
        default:
            break;
        }
    }
}

void thanks()
{
    cout << setw(consoleWidth - 1) << left << "*感谢你的使用" << "*" << endl;
}