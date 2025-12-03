#include "HSC/utils/RoutingParameter.hpp"
#include "meta/strncmp.hpp"

namespace hsc
{
    template<const char *Route>
    consteval bool RoutingParameter<Route>::IsValid()
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

    template<const char *Route>
    consteval size_t RoutingParameter<Route>::CountParameters()
    {
        size_t result = 0;

        for (size_t it = 0; Route[it]; it++)
            if (Route[it] == '{')
                result++;
        return result;
    }

    template<const char *Route>
    consteval std::array<std::pair<const char *, size_t>, RoutingParameter<Route>::params_size> RoutingParameter<Route>::GetParameters()
    {
        std::array<std::pair<const char *, size_t>, params_size> out{};
        size_t entry = 0;
        size_t index = 0;

        for (size_t it = 0; Route[it]; it++) {
            if (Route[it] == '{') {
                entry = it;
            } else if (Route[it] == '}') {
                out[index++] = {Route + entry + 1, it - entry - 1};
            }
        }
        return out;
    }

    template<const char *Route>
    consteval bool RoutingParameter<Route>::MatchParameter(const std::string_view &_param)
    {
        for (const std::pair<const char *, size_t> &_ptr : params_name) {
            if (_param.size() != _ptr.second)
                continue;
            if (meta::extra::strncmp(_param.data(), _ptr.first, _ptr.second))
                return true;
        }
        return false;
    }
}