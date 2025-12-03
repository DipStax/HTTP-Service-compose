#pragma once

namespace meta::extra
{
    constexpr bool strncmp(const char *_left, const char *_right, size_t _size)
    {
        for (size_t it = 0; it < _size; it++)
            if (_left[it] != _right[it])
                return false;
        return true;
    }
}