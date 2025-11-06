#if !defined CONTROLLER_H
#define CONTROLLER_H

#include <inttypes.h>

class Controller
{
	int16_t Vx_;
	int16_t Vy_;
	int16_t Vz_;
	int16_t Wx_;
	int16_t Wy_;
	int16_t Wz_;
public:
	Controller();

};

#endif