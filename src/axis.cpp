#include "axis.h"

#include <algorithm>

Axises::Axises() : axises{0, 0, 0, 0, 0, 0} {}

Axises::Axises(const int16_t* axises)
{
    for (char i = 0; i < 6; ++i)
    {
        this->axises[i] = std::clamp(axises[i], int16_t(-256), int16_t(256));
    }
}

int16_t& Axises::operator[](int index)
{
    return axises[index];
}

int16_t& Axises::operator[](AxisesNames index)
{
    return axises[static_cast<int>(index)];
}

int16_t Axises::operator[](AxisesNames index) const
{
    return axises[static_cast<int>(index)];
}

int16_t Axises::operator[](int index) const
{
    return axises[index];
}

Axises::Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz)
{
    (*this)[AxisesNames::Vx] = std::clamp(Vx, int16_t(-256), int16_t(256));
    (*this)[AxisesNames::Vy] = std::clamp(Vy, int16_t(-256), int16_t(256));
    (*this)[AxisesNames::Vz] = std::clamp(Vz, int16_t(-256), int16_t(256));
    (*this)[AxisesNames::Wx] = std::clamp(Wx, int16_t(-256), int16_t(256));
    (*this)[AxisesNames::Wy] = std::clamp(Wy, int16_t(-256), int16_t(256));
    (*this)[AxisesNames::Wz] = std::clamp(Wz, int16_t(-256), int16_t(256));
}

int Axises::getAxis(AxisesNames axis) const
{
    return axises[static_cast<int>(axis)];
}

int Axises::getAxis(int axis) const
{
    return axises[axis];
}