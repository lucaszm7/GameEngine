#pragma once

#include <vector>

namespace cgl
{
	template <typename _Type>
	class mat
	{
	private:
		_Type* matrix;
		unsigned int m_rowsSize = 0;
		unsigned int m_collumsSize = 0;

	public:
		mat() = default;

		mat(unsigned int _rows, unsigned int _collums)
			:m_rowsSize(_rows), m_collumsSize(_collums)
		{
			matrix = new _Type[m_rowsSize * m_collumsSize];
		}

		~mat()
		{
			delete[] matrix;
		}

		void resize(unsigned int _rows, unsigned int _collums)
		{
			if (_rows == m_rowsSize && _collums == m_collumsSize)
				return;

			m_rowsSize = _rows;
			m_collumsSize = _collums;

			delete[] matrix;

			matrix = new _Type[m_rowsSize * m_collumsSize];
		};

		_Type* data()
		{
			return matrix;
		}

		size_t row_size() const { return m_rowsSize; };
		size_t collum_size() const { return m_collumsSize; };
		size_t size() const { return m_rowsSize * m_collumsSize; };

		void set(unsigned int column, unsigned int row, const _Type& p)
		{
			matrix[column * m_rowsSize + row] = p;
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
