#include "listener.h"

#include "../core/buffer.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <array>

namespace myro
{
	struct listener_data
	{
		vec3 position;
		vec3 velocity;

		vec3 forward; // orientation[0]
		vec3 up; // orientation[1]
	};

	static listener_data s_data;

	namespace detail 
	{
		static std::array<float, 6> as_orientation(const vec3& forward, const vec3& up)
		{
			std::array<float, 6> result 
			{ 
				forward.x, forward.y, forward.z,
				up.x, up.y, up.z
			};

			return result;
		}
	}

	void listener::init()
	{
		s_data.position = { 0.0,0.0,0.0 };
		s_data.velocity = { 0.0,0.0,0.0 };
		s_data.forward = { 0.0,0.0,-1.0 };
		s_data.up = { 0.0, 1.0, 0.0 };

		alListenerfv(AL_POSITION, s_data.position.v);
		alListenerfv(AL_VELOCITY, s_data.velocity.v);
		alListenerfv(AL_ORIENTATION, detail::as_orientation(s_data.forward, s_data.up).data());
	}

	void listener::init(const vec3& position, const vec3& velocity, const vec3& forward, const vec3& up)
	{
		s_data.position = position;
		s_data.velocity = velocity;
		s_data.forward = forward;
		s_data.up = up;

		alListenerfv(AL_POSITION, s_data.position.v);
		alListenerfv(AL_VELOCITY, s_data.velocity.v);
		alListenerfv(AL_ORIENTATION, detail::as_orientation(s_data.forward, s_data.up).data());
	}

	void listener::set_position(const vec3& position)
	{
		s_data.position = position;

		alListenerfv(AL_POSITION, s_data.position.v);
	}

	void listener::set_velocity(const vec3& velocity)
	{
		s_data.velocity = velocity;

		alListenerfv(AL_VELOCITY, s_data.velocity.v);
	}

	void listener::set_forward(const vec3& forward)
	{
		s_data.forward = forward;

		alListenerfv(AL_ORIENTATION, detail::as_orientation(s_data.forward, s_data.up).data());
	}

	void listener::set_up(const vec3& up)
	{
		s_data.up = up;

		alListenerfv(AL_ORIENTATION, detail::as_orientation(s_data.forward, s_data.up).data());
	}

	void listener::set_orientation(const vec3& forward, const vec3& up)
	{
		s_data.forward = forward;
		s_data.up = up;

		alListenerfv(AL_ORIENTATION, detail::as_orientation(s_data.forward, s_data.up).data());
	}

	vec3 listener::get_position()
	{
		return s_data.position;
	}

	vec3 listener::get_velocity()
	{
		return s_data.velocity;
	}

	vec3 listener::get_forward()
	{
		return s_data.forward;
	}

	vec3 listener::get_up()
	{
		return s_data.up;
	}

}
