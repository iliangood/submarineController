#include "axis.h"


Axises::Axises() : axises_{0, 0, 0, 0, 0, 0} {}

Axises::Axises(const int16_t* axises)
{
    for (int i = 0; i < 6; ++i)
    {
        this->axises_[i] = axises[i];
    }
}

Axises::Axises(std::array<int16_t, 6> axises)
{
    for (int i = 0; i < 6; ++i)
    {
        this->axises_[i] = axises[i];
    }
}

int16_t& Axises::operator[](int index)
{
    return axises_[index];
}

int16_t& Axises::operator[](AxisesNames index)
{
    return axises_[static_cast<int>(index)];
}

int16_t Axises::operator[](AxisesNames index) const
{
    return axises_[static_cast<int>(index)];
}

int16_t Axises::operator[](int index) const
{
    return axises_[index];
}

Axises::Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz)
{
    (*this)[AxisesNames::Vx] = Vx;
    (*this)[AxisesNames::Vy] = Vy;
    (*this)[AxisesNames::Vz] = Vz;
    (*this)[AxisesNames::Wx] = Wx;
    (*this)[AxisesNames::Wy] = Wy;
    (*this)[AxisesNames::Wz] = Wz;
}

int16_t Axises::getAxis(AxisesNames axis) const
{
    return axises_[static_cast<int>(axis)];
}

int16_t Axises::axis(AxisesNames axis) const
{
    return axises_[static_cast<int>(axis)];
}

int16_t Axises::getAxis(int axis) const
{
    return axises_[axis];
}

int16_t Axises::axis(int axis) const
{
    return axises_[axis];
}