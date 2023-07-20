#pragma once

#include <vector>

namespace cgl
{
	template <typename _Type>
	class mat
	{
	private:
		_Type* matrix;
		unsigned int m_height = 0;
		unsigned int m_width = 0;

	public:
		mat() = default;

		// Height x Widht
		mat(unsigned int height, unsigned int width)
			:m_height(height), m_width(width)
		{
			matrix = new _Type[m_height * m_width];
		}

		~mat()
		{
			delete[] matrix;
		}

		// Height x Widht
		void resize(unsigned int height, unsigned int width)
		{
			if (height == m_height && width == m_width)
				return;
#ifdef _DEBUG
			std::cout << "resizing matrix of " << m_height << " x " << m_width << " to " << height << " x " << width << "\n";
#endif

			m_height = height;
			m_width = width;

			delete[] matrix;

			matrix = new _Type[m_height * m_width];
		};

		_Type* data()
		{
			return matrix;
		}

		size_t height() const { return m_height; };
		size_t width() const { return m_width; };
		size_t size() const { return m_height * m_width; };

		// y - x
		void set(unsigned int n_height, unsigned int n_width, const _Type& p)
		{
#ifdef _DEBUG
			if ((n_height * m_width + n_width) >= size())
			{
				std::cout << "Accesing data out of range in matrix...\n";
				__cpp_static_assert;
			}
#endif
			matrix[n_height * m_width + n_width] = p;
		}

		_Type get(unsigned int n_height, unsigned int n_width)
		{
#ifdef _DEBUG
			if ((n_height * m_width + n_width) >= size())
			{
				std::cout << "Accesing data out of range in matrix...\n";
				__cpp_static_assert;
			}
#endif
			return matrix[n_height * m_width + n_width];
		}

		void clear(const _Type& def)
		{
			for (unsigned int i = 0; i < size(); ++i)
			{
				matrix[i] = def;
			}
		}
	};
}
