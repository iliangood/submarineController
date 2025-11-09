#include "axis.h"

#include <cassert>



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

int16_t& Axises::operator[](std::size_t index)
{
	assert(index < 6 && "Axises::operator[](int index): incorrect index");
    return axises_[index];
}

int16_t& Axises::operator[](AxisesNames index)
{
	assert(static_cast<std::size_t>(index) < 6 && "Axises::operator[](AxisesNames index): incorrect index");
    return axises_[static_cast<int>(index)];
}

int16_t Axises::operator[](AxisesNames index) const
{
	assert(static_cast<std::size_t>(index) < 6 && "Axises::operator[](AxisesNames index) const: incorrect index");
    return axises_[static_cast<int>(index)];
}

int16_t Axises::operator[](std::size_t index) const
{
	assert(index < 6 && "Axises::operator[](int index) const: incorrect index");
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
	assert(static_cast<std::size_t>(axis) < 6 && "Axises::getAxis(std::size_t axis) const: incorrect axis");
    return axises_[static_cast<int>(axis)];
}

int16_t Axises::axis(AxisesNames axis) const
{
	assert(static_cast<std::size_t>(axis) < 6 && "Axises::axis(std::size_t axis) const: incorrect axis");
    return axises_[static_cast<int>(axis)];
}

int16_t Axises::getAxis(std::size_t axis) const
{
	assert(axis < 6 && "Axises::getAxis(std::size_t axis) const: incorrect axis");
    return axises_[axis];
}

int16_t Axises::axis(std::size_t axis) const
{
	assert(axis < 6 && "Axises::axis(std::size_t axis) const: incorrect axis");
    return axises_[axis];
}