//custom_vector.h
#pragma once
#ifndef _CUTOM_VECTOR_MODIFY_H_
#define _CUTOM_VECTOR_MODIFY_H_

#define _custom_type long long

#include <algorithm>
#include <istream>
#include <ostream>
#include <string>
#include <exception>

class custom_vector
{
public:
    custom_vector() : data(nullptr), size(0), capacity(0) {}
    custom_vector operator= (const custom_vector & right);
    _custom_type operator[](size_t offset);
    void sort();
    void sort(auto cmp = [](_custom_type x, _custom_type y)
              { return x > y; });
    void push_back(_custom_type value);
    void push_front(_custom_type value);
    void insert(size_t offset, _custom_type value);
    size_t find(_custom_type value, size_t offset = 0);
    void erase(size_t offset);
    _custom_type sum(size_t begin,size_t end);
    friend std::ostream &operator<<(std::ostream &os, custom_vector &v);
    friend std::istream &operator>>(std::istream &is, custom_vector &v);
    _custom_type * begin();
    _custom_type * rbegin();
    _custom_type * rend();
    _custom_type * end();
    size_t get_size() const;

protected:
    void update_capacity(size_t target_size);

private:
    _custom_type *data;
    size_t size;
    size_t capacity;
};

#include "custom_vector.cpp"
#endif