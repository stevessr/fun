#pragma once

// #define _custom_type long long

#include <algorithm>
#include <istream>
#include <ostream>
#include <string>
#include <exception>

template <typename _custom_type>
class custom_vector
{
public:
    custom_vector<_custom_type>::() : data(nullptr), size(0), capacity(0) {}
    ~custom_vector<_custom_type>::() { delete[] data; }
    custom_vector<_custom_type> operator=(const custom_vector<_custom_type>::&right);
    _custom_type operator[](size_t offset);
    void sort();
    void sort(auto cmp = [](_custom_type x, _custom_type y)
              { return x > y; });
    void push_back(_custom_type value);
    void push_front(_custom_type value);
    void insert(size_t offset, _custom_type value);
    size_t find(_custom_type value, size_t offset = 0);
    void erase(size_t offset);
    _custom_type sum(size_t begin, size_t end);
    friend std::ostream &operator<<(std::ostream &os, custom_vector<_custom_type>::&v);
    friend std::istream &operator>>(std::istream &is, custom_vector<_custom_type>::&v);
    _custom_type *begin();
    _custom_type *rbegin();
    _custom_type *rend();
    _custom_type *end();
    size_t get_size() const;

protected:
    void update_capacity(size_t target_size);

private:
    _custom_type *data;
    size_t size;
    size_t capacity;
};

template <typename _custom_type>
_custom_type custom_vector<_custom_type>::operator[](size_t offset)
{
    if (offset >= size)
    {
        throw std::out_of_range("Out of range");
    }
    return data[offset];
}
template <typename _custom_type>
void custom_vector<_custom_type>::push_back(_custom_type value)
{
    if (size >= capacity)
    {
        update_capacity(capacity == 0 ? 1 : capacity * 2);
    }
    data[size++] = value;
}
template <typename _custom_type>
void custom_vector<_custom_type>::push_front(_custom_type value)
{
    if (size >= capacity)
    {
        update_capacity(capacity == 0 ? 1 : capacity * 2);
    }
    for (size_t i = size; i > 0; i--)
    {
        data[i] = data[i - 1];
    }
    data[0] = value;
    size++;
}
template <typename _custom_type>
void custom_vector<_custom_type>::insert(size_t offset, _custom_type value)
{
    if (offset > size)
    {
        throw std::out_of_range("out of range");
    }
    if (size >= capacity)
    {
        update_capacity(capacity == 0 ? 1 : capacity * 2);
    }
    for (size_t i = size; i > offset; i--)
    {
        data[i] = data[i - 1];
    }
    data[offset] = value;
    size++;
}
template <typename _custom_type>
size_t custom_vector<_custom_type>::find(_custom_type value, size_t offset)
{
    for (size_t i = offset; i < size; i++)
    {
        if (data[i] == value)
        {
            return i;
        }
    }
    return size_t(-1);
}
template <typename _custom_type>
void custom_vector<_custom_type>::erase(size_t offset)
{
    if (offset >= size)
    {
        return;
    }
    for (size_t i = offset; i < size - 1; i++)
    {
        data[i] = data[i + 1];
    }
    size--;
}
template <typename _custom_type>
std::ostream &operator<<(std::ostream &os, custom_vector<_custom_type> &v)
{
    for (size_t i = 0; i < v.size; i++)
    {
        os << v[i] << " ";
    }
    return os;
}
template <typename _custom_type>
std::istream &operator>>(std::istream &is, custom_vector<_custom_type> &v)
{
    for (size_t i = 0; i < v.size; i++)
    {
        is >> v.data[i];
    }
    return is;
}
template <typename _custom_type>
_custom_type *custom_vector<_custom_type>::begin()
{
    return data;
}
template <typename _custom_type>
_custom_type *custom_vector<_custom_type>::end()
{
    return data + size;
}
template <typename _custom_type>
_custom_type *custom_vector<_custom_type>::rbegin()
{
    return data + size - 1;
}
template <typename _custom_type>
_custom_type *custom_vector<_custom_type>::rend()
{
    return data - 1;
}

template <typename _custom_type>
void custom_vector<_custom_type>::update_capacity(size_t target_size)
{
    if (target_size < 0)
    {
        return;
    }
    else if (target_size <= capacity)
    {
        return;
    }
    else
    {
        _custom_type *temp = new _custom_type[target_size];
        for (size_t i = 0; i < size; i++)
        {
            temp[i] = data[i];
        }
        delete[] data;
        data = temp;
        capacity = target_size;
    }
}

template <typename _custom_type>
custom_vector<_custom_type> custom_vector<_custom_type>::operator=(const custom_vector<_custom_type>::&right)
{
    if (this == &right)
    {
        return *this;
    }

    update_capacity(right.capacity);
    size = right.size;

    for (size_t i = 0; i < size; ++i)
    {
        data[i] = right.data[i];
    }

    return *this;
}

template <typename _custom_type>
void custom_vector<_custom_type>::sort()
{
    std::sort(data, data + size);
}

template <typename _custom_type>
void custom_vector<_custom_type>::sort(auto cmp)
{
    std::sort(data, data + size, cmp);
}

template <typename _custom_type>
size_t custom_vector<_custom_type>::get_size() const
{
    return size;
}

template <typename _custom_type>
_custom_type custom_vector<_custom_type>::sum(size_t begin, size_t end)
{
    _custom_type all = 0;
    if (begin < 0 || end >= size)
    {
        throw std::out_of_range("out of range");
    }
    while (begin <= end)
    {
        all += data[begin++];
    }
    return all;
}