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
    cout << setw(consoleWidth - 1) << left << "*��ӭ����Сѧ������ϰ��" << "*" << endl
         << setw(consoleWidth - 1) << left << "*��������C++����" << "*" << endl
         << setw(consoleWidth - 1) << left << "*���ܣ����Խ��мӡ������ˡ���������ϰ������������" << "*" << endl
         << setw(consoleWidth - 1) << left << "*��ϰ��������Ϸ�͡���ͨ��ϰ��" << "*" << endl;
    c.printline(cout);
}

void mode_get()
{
    cout << setw(consoleWidth - 1) << left << "*��ѡ����ϰģʽ��" << "*" << endl
         << setw(consoleWidth - 1) << left << "*1.��Ϸ��" << "*" << endl
         << setw(consoleWidth - 1) << left << "*2.��ͨ��ϰ��" << "*" << endl;
    cout << "*���ѡ����(����0�˳���Ϸ)��";
    cin >> temp;
    while (temp != "1" && temp != "2" && temp != "0")
    {
        cout << "*����������������룺";
        cin >> temp;
    }
    if (temp == "1")
    {
        mode = 1;
        cout << setw(consoleWidth - 1) << left << "*��Ϸģʽ�ѿ���" << "*" << endl;
    }
    else if (temp == "2")
    {
        mode = 0;
        cout << setw(consoleWidth - 1) << left << "*��ͨ��ϰģʽ�ѿ���" << "*" << endl;
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
    cout << setw(consoleWidth - 1) << left << "*���ѽ�����Ϸģʽ" << "*" << endl;
    double towait;
    cout << "*������ÿ�������ʱ����λ:�룩��";
    auto start = std::chrono::high_resolution_clock::now();
    cin >> towait;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << setw(consoleWidth - 1) << left << "*�޾����ؼ�����ʼ" << "*" << endl;
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
        cout << "*�������(ȡ��������)��";
        start = std::chrono::high_resolution_clock::now();
        cin >> temp;
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << setw(consoleWidth - 1) << left << "*��ʱ: " + to_string(duration) + "����" << "*" << endl;
        temp >> input;
        if (input == (c.result))
        {
            cout << setw(consoleWidth - 1) << left << "*����ȷ" << "*" << endl;
            cout << "*";
            system("pause");
            system("cls");
        }
        else
        {
            cout << setw(consoleWidth - 1) << left << "*�𰸴�����Ϸ��ֹ����ȷ��" + to_string(int(c.result)) << "*" << endl;
            break;
        }
        if (duration > 1000 * towait)
        {
            cout << setw(consoleWidth - 1) << left << "*��ʱ����Ϸ��ֹ" << "*" << endl;
            break;
        }
        c.printline(cout);
        a++;
    }
    cout << setw(consoleWidth - 1) << left << "*�ۼƳɹ�" + to_string(a) + "��" << "*" << endl;
}

void pratise()
{
    cout << "*��ѡ������������Ŀ���ͣ�1���ӷ���2��������3���˷���4��������5����ϣ�q���˳�����";
    cin >> temp;
    while (temp != "5" && temp != "4" && temp != "3" && temp != "2" && temp != "1" && temp != "q")
    {
        cout << "*�����������������:" << endl;
        cin >> temp;
    }
    if (temp == "q")
    {
        mode = -1;
        return;
    }
    long long towait, tars;
    cout << "*���������⣺";
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
        cout << "*�������(ȡ��������,����q�˳�)��";
        start = std::chrono::high_resolution_clock::now();
        cin >> temp;
        end = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << setw(consoleWidth - 1) << left << "*��ʱ: " + to_string(duration) + "����" << "*" << endl;
        temp >> input;
        if (input == int(c.result))
        {
            cout << setw(consoleWidth - 1) << left << "*����ȷ" << "*" << endl;
            a++;
        }
        else if (temp == "q")
        {
            mode = -1;
            break;
        }
        else
        {
            cout << setw(consoleWidth - 1) << left << "*�𰸴�����ȷ��" + to_string(int(c.result)) << "*" << endl;
        }
    }
    c.printline(cout);
    cout << setw(consoleWidth - 1) << left << "*�ۼƳɹ�" + to_string(a) + "��" << "*" << endl;
    cout << setw(consoleWidth - 1) << left << "*�ɹ���" + to_string(a * 1.0 / tars) + "%" << "*" << endl;
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
    cout << setw(consoleWidth - 1) << left << "*��л���ʹ��" << "*" << endl;
}