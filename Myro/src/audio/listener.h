#pragma once

#include "../math/vec3.h"

namespace myro
{
	class listener
	{
	public:
		static void init();
		static void init(const vec3& position, const vec3& velocity, const vec3& forward, const vec3& up);

		static void set_position(const vec3& position);
		static void set_velocity(const vec3& velocity);

		static void set_forward(const vec3& forward);
		static void set_up(const vec3& up);
		static void set_orientation(const vec3& forward, const vec3& up);

		static vec3 get_position();
		static vec3 get_velocity();

		static vec3 get_forward();
		static vec3 get_up();
	};
}