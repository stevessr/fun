// demo.cpp
#include <limits>
#include "custom_vector.h"
#include "ui.cpp"
#include <iostream>
using namespace std;

custom_vector v;

void _on_cin_fail()
{
    if (cin.fail())
        _clean_cin_buff();
}

void _clean_cin_buff()
{
    cin.clear();                                         // Clear the error flags
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the invalid input
}

void current_situation()
{
    print_line("当前元素数：" + to_string(v.get_size()));
    cout << "*当前元素：" << endl;
    for (size_t i = 0; i < v.get_size(); i++)
    {
        cout << v[i] << " ";
    }
    cout << endl;
}

void input_1()
{
    long long temp;
    input_line("请输入元素(非数字结束）：");
    while (cin >> temp)
    {
        v.push_back(temp);
    }
    _on_cin_fail();
}

void input_2()
{
    size_t nums;
    long long temp;
    input_line("请输入需要插入的数字数:");
    cin >> nums;
    input_line("请输入需要插入的数字:");
    while (nums--)
    {
        cin >> temp;
        v.push_back(temp);
    }
    _clean_cin_buff();
}

void sort()
{
    v.sort();
}

void insert()
{
    input_line("请输入插入位置：");
    size_t offset;
    cin >> offset;
    _clean_cin_buff();
    input_line("请输入插入元素：");
    long long temp;
    cin >> temp;
    _clean_cin_buff();
    try
    {
        v.insert(offset, temp);
    }
    catch (exception &e)
    {
        print_line("插入失败");
    }
}

void erase()
{
    input_line("请输入删除位置：");
    size_t offset;
    cin >> offset;
    try
    {
        v.erase(offset);
    }
    catch (exception &e)
    {
        print_line("删除失败");
    }
}

void find()
{
    input_line("请输入查找元素：");
    long long temp;
    cin >> temp;
    size_t offset = v.find(temp);
    if (offset == size_t(-1))
    {
        print_line("没有结果");
        return;
    }
    while (offset != size_t(-1))
    {
        print_line("查找结果：" + to_string(offset));
        offset = v.find(temp, offset + 1);
    }
}

void pos()
{
    input_line("请输入查找位置：");
    size_t offset;
    cin >> offset;
    try
    {
        print_line("查找结果：" + to_string(v[offset]));
    }
    catch (exception &e)
    {
        print_line("查找失败");
    }
}

void sum1()
{
    input_line("请输入求和开始位置");
    size_t begin_offset;
    cin >> begin_offset;
    input_line("请输入求和结束位置");
    size_t end_offset;
    cin >> end_offset;
    try
    {
        print_line("求和结果：" + to_string(v.sum(begin_offset, end_offset)));
    }
    catch (exception &e)
    {
        print_line("求和失败");
    }
}

void sum2()
{
    cout << "请输入求和位置(非数字结束）：";
    custom_vector temp;
    long long t;
    while (cin >> t)
    {
        temp.push_back(t);
    }
    _on_cin_fail();
    try
    {
        print_line("求和结果：" + to_string(v.sum(temp)));
    }
    catch (exception &e)
    {
        print_line("求和失败");
    }
}

void introduction()
{
    print_line("功能:");
    print_line("0.退出 ");
    print_line("1.数组输入 ");
    print_line("2.数组排序 ");
    print_line("3.元素插入 ");
    print_line("4.元素查找 ");
    print_line("5.元素删除 ");
    print_line("6.数组输出 ");
    print_line("7.输出指定位置元素 ");
    print_line("8.对指定始末位置的数组元素求和");
    print_line("9.数组状态查询");
    print_line("10.数组自由插入");
    print_line("11.对于若干选定位置的元素求和");
    print_line("p.s.输入的位置是0-based");
}

void choice()
{
    short choice = 9;
    while (choice != 0)
    {
        introduction();
        printline();
        input_line("你的选择是：");
        cin >> choice;
        _clean_cin_buff();
        switch (choice)
        {
        case 1:
            input_2();
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
            current_situation();
            break;
        case 10:
            input_1();
            break;
        case 11:
            sum2();
            break;
        case 0:
            break;
        default:
            break;
        }
        _on_cin_fail();
        printline();
    }
}

int main()
{
    choice();
    return 0;
}
