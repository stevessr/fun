#pragma once
#ifndef CUSTOM_VECTOR_CPP
#define CUSTOM_VECTOR_CPP
#include "custom_vector.h"

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