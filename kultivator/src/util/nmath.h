#ifndef NMATH_H
#define NMATH_H

// custom utility math functions
namespace nmath {

inline constexpr double lerp(double min, double max, double t)
{
    return (min * (1.0 - t)) + (max * t); // more accurate than min + t(max - min)
}

inline constexpr double invLerp(double min, double max, double val)
{
    return (val - min) / (max - min); // divide by zero? idk see what happens
}
}

#endif // NMATH_H
