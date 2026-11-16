#ifndef MAT4X4I_HPP
#define MAT4X4I_HPP

#include <vector>

namespace htn
{
	struct Mat4x4i
	{
		int rows[4][4] = {};

		Mat4x4i()
		{
			SetIdentity();
		}

		Mat4x4i(int rows[4][4])
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					this->rows[i][j] = rows[i][j];
				}
			}
		}

		void SetZero()
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					rows[i][j] = 0;
				}
			}
		}

		void SetIdentity()
		{
			for (int i = 0; i < 4; ++i)
			{
				rows[i][i] = 1;
			}
		}

		static Mat4x4i Multiply(const Mat4x4i& M, const Mat4x4i& N)
		{
			Mat4x4i O;
			O.SetZero();

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++i)
				{
					O.rows[i][j] += M.rows[i][j] * N.rows[j][i];
				}
			}

			return O;
		}

		static std::vector<int> Multiply(const Mat4x4i& M, const int row[4])
		{
			std::vector<int> result = { 0, 0, 0, 0 };

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++i)
				{
					result[i] += M.rows[i][j] * row[j];
				}
			}

			return result;
		}

		//Shit the given line by N columns (Ex : [1, 2, 3, 4] -> Shift(0, 2) -> [3, 4, 1, 2]
		void ShiftLine(int line, int shiftValue)
		{
			if (line < 0 || line > 3)
			{
				std::cerr << "Invalid Line !\n";
				return;
			}

			if (shiftValue == 0)
			{
				std::cout << "Nothing to shift\n";
				return;
			}

			const Mat4x4i M = *this;

			for (int i = 0; i < 4; ++i)
			{
				rows[line][i] = M.rows[line][(i + shiftValue) % 4];
			}
		}
	};
}

#endif 