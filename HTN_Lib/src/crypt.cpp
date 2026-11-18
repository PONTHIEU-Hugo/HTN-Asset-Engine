#include "crypt.h"
#include <cstdint>

namespace htn
{
	MatGF4x4 Crypt::mAuxiliar = {};
	MatGF4x4 Crypt::mInverseAuxiliar = {};

	void Crypt::InitAuxiliar()
	{
		uint8_t rows[4][4] = {
			{2, 3, 1 ,1},
			{1, 2, 3, 1},
			{1, 1, 2 ,3},
			{3, 1, 1, 2}
		};

		mAuxiliar = MatGF4x4(rows);

		uint8_t invRows[4][4] = {
			{14, 11, 13, 9},
			{9, 14, 11, 13},
			{13, 9, 14, 11},
			{11, 13, 9, 14}
		};

		mInverseAuxiliar = MatGF4x4(invRows);
	}

	std::string Crypt::AES_Encrypt(char* data, size_t size, std::string key)
	{
		if (mAuxiliar.rows[0][0] == 1)
			InitAuxiliar();

		size_t mat_amounts = 1 + (size - 1) / 16;

		std::string output = {};
		output.resize((size+ 15) / 16 * 16);

		const int old_key_size = key.size();
		int padding = (16 - old_key_size) % 16;

		if (padding < 0)
			padding = (16 + padding) % 16;

		//fill the key if not a multiple of 16 chars
		for (int i = 0; i < padding; ++i)
		{
			key.push_back(key[i % old_key_size]);
		}

		for (size_t n = 0; n < mat_amounts; ++n)
		{
			size_t mat_offset = n * 16;

			MatGF4x4 m = {};

			//fill data
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; ++j)
				{
					size_t offset = mat_offset + i * 4 + j;

					if (offset >= size)
						m.rows[i][j] = 0;
					else
						m.rows[i][j] = static_cast<uint8_t>(data[offset]);
				}
			}

			//shift the i lines i time to the left
			for (int i = 0; i < 4; ++i)
			{
				m.ShiftLine(i, -i);
			}

			//mix columns
			for (int i = 0; i < 4; ++i)
			{
				std::vector<uint8_t> result = {};

				for (int j = 0; j < 4; ++j)
					result.push_back(m.rows[j][i]);

				result = MatGF4x4::Multiply(mAuxiliar, result);

				for (int j = 0; j < 4; ++j)
					m.rows[j][i] = result[j];
			}

			//xor with keys
			size_t xor_iterations = key.size() / 16;
			std::vector<std::vector<std::string>> pieces = {};

			for (int i = 0; i < xor_iterations; ++i)
			{
				std::vector<std::string> piece = {};
				for (int j = 0; j < 4; ++j)
				{
					piece.push_back(key.substr(i*16 + j*4, 4));
				}

				pieces.push_back(piece);
			}

			for (int i = 0; i < xor_iterations; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					m.XOR_Column(j, pieces[i][j]);
				}
			}

			//set data
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; ++j)
				{
					size_t offset = mat_offset + i * 4 + j;
					output[offset] = m.rows[i][j];
				}
			}
		}

		return output;
	}

	std::string Crypt::AES_Decrypt(char* data, size_t size, std::string key)
	{
		if (mAuxiliar.rows[0][0] == 1)
			InitAuxiliar();

		size_t mat_amounts = 1 + (size - 1) / 16;

		std::string output = {};
		output.resize((size + 15) / 16 * 16);

		const int old_key_size = key.size();
		int padding = (16 - old_key_size) % 16;

		if (padding < 0)
			padding = (16 + padding) % 16;

		for (int i = 0; i < padding; ++i)
		{
			key.push_back(key[i % old_key_size]);
		}

		for (size_t n = 0; n < mat_amounts; ++n)
		{
			size_t mat_offset = n * 16;

			MatGF4x4 m = {};

			//fill data
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; ++j)
				{
					size_t offset = mat_offset + i * 4 + j;
					m.rows[i][j] = static_cast<uint8_t>(data[offset]);
				}
			}

			//xor with keys
			size_t xor_iterations = key.size() / 16;
			std::vector<std::vector<std::string>> pieces = {};

			for (int i = 0; i < xor_iterations; ++i)
			{
				std::vector<std::string> piece = {};
				for (int j = 0; j < 4; ++j)
				{
					piece.push_back(key.substr(i * 16 + j * 4, 4));
				}

				pieces.push_back(piece);
			}

			for (int i = 0; i < xor_iterations; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					m.XOR_Column(j, pieces[i][j]);
				}
			}

			//mix columns
			for (int i = 0; i < 4; ++i)
			{
				std::vector<uint8_t> result = {};

				for (int j = 0; j < 4; ++j)
					result.push_back(m.rows[j][i]);

				result = MatGF4x4::Multiply(mInverseAuxiliar, result);

				for (int j = 0; j < 4; ++j)
					m.rows[j][i] = result[j];
			}

			//shift the i lines i time to the right
			for (int i = 0; i < 4; ++i)
			{
				m.ShiftLine(i, i);
			}

			//set data
			for (size_t i = 0; i < 4; ++i)
			{
				for (size_t j = 0; j < 4; ++j)
				{
					size_t offset = mat_offset + i * 4 + j;		
					output[offset] = m.rows[i][j];
				}
			}
		}

		output.resize(size);
		return output;
	}
}