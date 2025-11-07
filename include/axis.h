#if !defined (AXIS_H)
#define AXIS_H

#include <inttypes.h>
#include <array>

enum class AxisesNames
{
    Vx = 0,
    Vy = 1,
    Vz = 2,
    Wx = 3,
    Wy = 4,
    Wz = 5
};

class Axises
{
    int16_t axises_[6];
public:
    Axises();

    Axises(const int16_t* axises);

    Axises(std::array<int16_t, 6> axises);

    int16_t& operator[](int index);

    int16_t& operator[](AxisesNames index);

    int16_t operator[](AxisesNames index) const;

    int16_t operator[](int index) const;

    Axises(int16_t Vx, int16_t Vy, int16_t Vz, int16_t Wx, int16_t Wy, int16_t Wz);

    int16_t getAxis(AxisesNames axis) const;
	int16_t axis(AxisesNames axis) const;

    int16_t getAxis(int axis) const;
	int16_t axis(int axis) const;
};

#endif