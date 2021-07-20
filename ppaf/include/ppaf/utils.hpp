#include <atomic>

template <typename T>
inline constexpr int sign(T val)
{
    return (T(0) < val) - (val < T(0));
}

template <typename T>
inline constexpr T moveto(T val, T dest, T delta)
{
    return std::abs(dest - val) <= delta ? dest :
        val + sign(dest - val) * delta;
}

inline void atomicFloatInc(std::atomic<double>& to, double inc)
{
    double current = to.load();
    while (!to.compare_exchange_weak(current, current + inc));
}