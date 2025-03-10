#pragma once
#ifndef CUSTOM_VECTOR_CPP
#define CUSTOM_VECTOR_CPP
#include "custom_vector.h"

_custom_type custom_vector::operator[](size_t offset)
{
    if(offset >= size){
        throw std::out_of_range("Out of range");
    }
    return data[offset];
}
void custom_vector::push_back(_custom_type value)
{
    if (size >= capacity)
    {
        update_capacity(capacity * 2);
    }
    data[size++] = value;
}
void custom_vector::push_front(_custom_type value)
{
    if (size >= capacity)
    {
        update_capacity(capacity * 2);
    }
    for (size_t i = size; i > 0; i--)
    {
        data[i] = data[i - 1];
    }
    data[0] = value;
    size++;
}
void custom_vector::insert(size_t offset, _custom_type value)
{
    if (size >= capacity)
    {
        update_capacity(capacity * 2);
    }
    for (size_t i = size; i > offset; i--)
    {
        data[i] = data[i - 1];
    }
    data[offset] = value;
    size++;
}
size_t custom_vector::find(_custom_type value, size_t offset = 0)
{
    for (size_t i = 0; i < size; i++)
    {
        if (data[i] == value)
        {
            return i;
        }
    }
    return -1;
}
void custom_vector::earse(size_t offset) {
    if(offset >= size){
        return;
    }
    for (size_t i = offset; i < size - 1; i++)
    {
        data[i] = data[i + 1];
    }
    size--;
}
std::ostream &operator<<(std::ostream &os, custom_vector &v) {
    for(size_t i = 0; i < v.size; i++){
        os << v[i] << " ";
    }
    return os;
}
std::istream &operator>>(std::istream &is, custom_vector &v) {
    for(size_t i = 0; i < v.size; i++){
        is >> v.data[i];
    }
    return is;
}
_custom_type *custom_vector::begin() {
    return data;
}
_custom_type *custom_vector::end() {
    return data + size;
}
_custom_type *custom_vector::rbegin() {}
_custom_type *custom_vector::rend() {}

void custom_vector::update_capacity(size_t target_size)
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
    }
}
#undef _custom_type
#endif