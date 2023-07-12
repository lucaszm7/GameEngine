#pragma once

#include <vector>

namespace cgl
{
	template <typename _Type>
	class mat
	{
	private:
		std::vector<std::vector<_Type>> matrix;
		unsigned int m_rowsSize = 0;
		unsigned int m_collumsSize = 0;

	public:
		mat() = default;

		mat(unsigned int _rows, unsigned int _collums)
			:m_rowsSize(_rows), m_collumsSize(_collums)
		{
			matrix.resize(m_rowsSize);
			for (auto& row : matrix)
			{
				row.resize(m_collumsSize);
			}
		}

		void resize(unsigned int _rows, unsigned int _collums)
		{
			if (_rows == m_rowsSize && _collums == m_collumsSize)
				return;

			m_rowsSize = _rows;
			m_collumsSize = _collums;

			matrix.resize(m_rowsSize);
			for (auto& row : matrix)
			{
				row.resize(m_collumsSize);
			}
		};

		size_t RowSize() const { return m_rowsSize; };
		size_t CollumSize() const { return m_collumsSize; };
		size_t size() const { return m_rowsSize * m_collumsSize; };
		void clear(const _Type& def)
		{
			for (auto& rows : matrix)
			{
				std::ranges::fill(rows, def);
			}
		}

		std::vector<_Type>  operator [] (unsigned int i) const { return matrix[i]; };
		std::vector<_Type>& operator [] (unsigned int i)       { return matrix[i]; };
	};
}
