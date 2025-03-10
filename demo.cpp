// demo.cpp
#include "custom_vector.h"
#include "ui.cpp"
#include <iostream>
using namespace std;

custom_vector v;

void current_situation()
{
    cout << "当前元素数：" << v.get_size() << endl;
    cout << "当前元素：" << endl;
    for (size_t i = 0; i < v.get_size(); i++)
    {
        cout << i << " ";
    }
}

void input_1()
{
    long long temp;
    cout << "请输入元素(非数字结束）：" << endl;
    while (cin >> temp)
    {
        v.push_back(temp);
    }
}

void input_2()
{
    size_t nums;
    long long temp;
    cout << "请输入需要插入的数字数:";
    cin >> nums;
    cout << "请输入需要插入的数字:" << endl;
    while (nums--)
    {
        cin >> temp;
        v.push_back(temp);
    }
}

void sort()
{
    v.sort();
}

void insert()
{
    cout << "请输入插入位置：";
    size_t offset;
    cin >> offset;
    cout << "请输入插入元素：";
    long long temp;
    cin >> temp;
    v.insert(offset, temp);
}

void erase()
{
    cout << "请输入删除位置：";
    size_t offset;
    cin >> offset;
    v.erase(offset);
}

void find()
{
    cout << "请输入查找元素：";
    long long temp;
    cin >> temp;
    size_t offset = v.find(temp);
    if (offset == size_t(-1))
    {
        cout << "没有结果" << endl;
        return;
    }
    while (offset != size_t(-1))
    {
        cout << "查找结果：" << offset << endl;
        offset = v.find(temp);
    }
}

void pos()
{
    cout << "请输入查找位置：";
    size_t offset;
    cin >> offset;
    cout << "查找结果：" << v[offset] << endl;
}

void sum1()
{
    cout << "请输入求和开始位置";
    size_t begin_offset;
    cin >> begin_offset;
    cout << "请输入求和结束位置";
    size_t end_offset;
    cin >> end_offset;
    cout << "求和结果：" << v.sum(begin_offset, end_offset) << endl;
}

void introduction()
{
    cout << "功能:\n\
0.退出 \n\
1.数组输入 \n\
2.数组排序 \n\
3.元素插入 \n\
4.元素查找 \n\
5.元素删除 \n\
6.数组输出 \n\
7.输出指定位置元素 \n\
8.对指定个数的数组元素求和\n\
9.数组状态查询\n\
10.数组自由插入\
";
}

void choice()
{
    short choice = 9;
    while (choice != 0)
    {
        introduction();
        printline();
        cout << "你的选择是：";
        cin >> choice;
        switch (choice)
        {
        case 1:
            input_1();
            break;
        case 2:
            sort();
            break;
        case 3:
            insert();
            break;
        case 4:
            find();
            break;
        case 5:
            erase();
            break;
        case 6:
            current_situation();
            break;
        case 7:
            pos();
            break;
        case 8:
            sum1();
            break;
        case 9:
            input_2();
            break;
        case 10:
            current_situation();
            break;
        case 0:
            break;
        default:
            break;
        }
    }
}

int main()
{
    choice();
    return 0;
}