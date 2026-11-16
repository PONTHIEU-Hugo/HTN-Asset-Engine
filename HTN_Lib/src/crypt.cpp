#include "crypt.h"

namespace htn
{
	Mat4x4i Crypt::mAuxiliar = {};

	void Crypt::Encrypt(char* data, uint64_t size, std::string key)
	{
		const size_t key_size = key.size();

		for (size_t i = 0; i < size; ++i)
		{
			char c = key[i % key_size];

			c =  1 + c % 7;

			data[i] += c;
		}
	}

	void Crypt::Decrypt(char* encrypted, uint64_t size, std::string key)
	{
		const size_t key_size = key.size();

		for (size_t i = 0; i < size; ++i)
		{
			char c = key[i % key_size];

			c = 1 + c % 7;

			encrypted[i] -= c;
		}
	}

	void Crypt::InitAuxiliar()
	{
		int rows[4][4] = {
			{2, 3, 1 ,1},
			{1, 2, 3, 1},
			{1, 1, 2 ,3},
			{3, 1, 1, 2}
		};

		mAuxiliar = Mat4x4i(rows);
	}

	void Crypt::AES_Encrypt(char* data, size_t size, std::string key)
	{
		if (mAuxiliar.rows[0][0] == 0)
			InitAuxiliar();


	}

	void Crypt::AES_Decrypt(char* data, size_t size, std::string key)
	{

	}
}