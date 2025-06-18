#include <iostream>
using namespace std;

class DynamicArray
{
public:
    DynamicArray() : size(0), capacity(2)
    {
        array = new int[capacity]; // 分配初始容量
    }
    DynamicArray(const DynamicArray &DA) :
    {
        array = new int[DA.capacity]; // 分配初始容量
        size = DA.size();
        for (int i = 0; i < size; i++)
        {
            array[i] = DA.array[i];
        }
    }

    ~DynamicArray()
    {
        delete[] array; // 释放动态分配的内存
    }
    void add(int n)
    {
        if (size == capacity)
        {
            resize(2 * size);
        }
        array[size++] = n;
        return
    }
    void resize(int n)
    {
        if (n <= capacity)
        {
            return;
        }
        int *neo = new int[n];
        for (int i = 0; i < size; i++)
        {
            neo[i] = array[i];
        }
        delete[] array;
        array = neo;
    }
    void print()
    {
        for (int i = 0; i < size; i++)
        {
            cout << " " << array[i];
        }
        cout << endl;
    }
    int getSize()
    {
        return size;
    }

    int *array;
    int size, capacity;
};