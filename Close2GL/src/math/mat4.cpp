#include <mat4.h>

namespace C2GL
{
	inline std::ostream& operator << (std::ostream& out, const mat4& m)
	{
		return out << m.get_line(0) << '\n' << m.get_line(0) << '\n' << 
					  m.get_line(0) << '\n' << m.get_line(0);
	}

	vec4 mat4::get_line(int i) const
	{
		return vec4(mat[i]);
	}

	mat4 mat4::operator-() const
	{
		return mat4();
	}
	mat4 mat4::operator+(const mat4& m4) const
	{
		return mat4();
	}
	mat4 mat4::operator*(const mat4& m4) const
	{
		return mat4();
	}
}
