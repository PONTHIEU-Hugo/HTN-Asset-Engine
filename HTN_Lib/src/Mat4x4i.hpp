#ifndef MAT4X4I_HPP
#define MAT4X4I_HPP

#include <vector>
#include <iostream>

namespace htn
{
	//GF(2^8) Galois -> [0, 255]
	struct MatGF4x4
	{
		uint8_t rows[4][4] = {};

		MatGF4x4()
		{
			SetIdentity(); 
		}

		MatGF4x4(uint8_t rows[4][4])
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

		static std::vector<uint8_t> Multiply(const MatGF4x4& M, std::vector<uint8_t> row)
		{
			auto mul = [&](uint8_t a, uint8_t b) -> uint8_t
				{
					uint8_t result = 0;

					//a * b in mod 2 -> same logic as mod 10
					
					//5 * 694  = 5 * 6*10^2 + 5 * 9*10^1 + 5*4*10^0
					//a * 1101 = a * 1*2^3 + a * 1*2^2 + a * 0*2^1 + a * 1*2^0

					for (int i = 0; i < 8; ++i)
					{
						if (b & 1)
							result ^= a;

						//apply the modulo 0x11b for AES when a == 2^7
						bool isTooLarge = (a & 0x80);

						//double
						a <<= 1;

						if (isTooLarge)
							a ^= 0x11b;

						b >>= 1;
					}

					return result;
				};

			std::vector<uint8_t> result = { 0, 0, 0, 0 };

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result[i] ^= mul(M.rows[i][j], row[j]);
				}
			}

			return result;
		}

		void XOR_Column(int column, std::string key)
		{
			if (column < 0 || column > 4)
			{
				std::cerr << "Invalid Column !\n";
				return;
			}

			for (int j = 0; j < 4; ++j)
			{
				rows[j][column] ^= static_cast<uint8_t>(key[j]);
			}
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
				return;

			const MatGF4x4 M = *this;

			for (int i = 0; i < 4; ++i)
			{
				int offset = 0;
				
				offset = (i + shiftValue) % 4;
				
				if (shiftValue < 0)
					offset = (4 + offset) % 4;

				rows[line][offset] = M.rows[line][i];
			}
		}
	};
}

#endif 