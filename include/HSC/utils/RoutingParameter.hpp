#pragma once

#include <vector>
#include <string>

template<const char *Route>
struct RoutingParameter
{
    static consteval bool IsValid()
    {
        bool finish = true;
        size_t entry = 0;

        for (size_t it = 0; Route[it]; it++) {
            if (Route[it] == '{') {
                entry = it;
                finish = false;
            } else if (Route[it] == '}') {
                if (finish)
                    return false;
                finish = true;
            } else if (Route[it] == ' ' && !finish) {
                return false;
            } else if (!finish) {
                if ((
                        (Route[it] < 'a' || 'z' < Route[it])
                        && (Route[it] < 'A' || 'Z' < Route[it])
                        && Route[it] != '_'
                    ) && (
                        (entry + 1 == it && ('0' <= Route[it] && Route[it] <= '9'))
                        || (Route[it] < '0' || '9' < Route[it])
                    ))
                    return false;
            }
        }
        return finish;
    }

    static_assert(IsValid(), "The route is invalid");

    static consteval size_t CountParameters()
    {
        size_t result = 0;

        for (size_t it = 0; Route[it]; it++)
            if (Route[it] == '{')
                result++;
        return result;
    }

    static constexpr size_t params_size = CountParameters();

    static consteval std::array<std::pair<const char *, size_t>, params_size> GetParameters()
    {
        std::array<std::pair<const char *, size_t>, params_size> out{};
        size_t entry = 0;
        size_t index = 0;

        for (size_t it = 0; Route[it]; it++) {
            if (Route[it] == '{') {
                entry = it;
            } else if (Route[it] == '}') {
                out[index++] = {Route + entry + 1, entry - it - 1};
            }
        }
        return out;
    }

    static constexpr bool MatchParameter(const std::string_view &_param)
    {
        for (const std::pair<const char *, size_t> &_ptr : params_name) {
            if (_param.size() != _ptr.second)
                continue;
            if (std::strncmp(_param.data(), _ptr.first, _ptr.second) == 0)
                return true;
        }
        return false;
    }

    static constexpr std::array<std::pair<const char *, size_t>, params_size> params_name = GetParameters();
};
