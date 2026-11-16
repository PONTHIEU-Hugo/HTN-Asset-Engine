#include "AssetEngine.h"

int main()
{
	std::string txt = "Yo la team comment ca va ? :3";
	std::cout << "Start : " << txt << std::endl;

	htn::Crypt::Encrypt(txt.data(), txt.size(), "Hugo Decrypt");
	std::cout << "Encrypted : " << txt << std::endl;

	htn::Crypt::Decrypt(txt.data(), txt.size(), "Hugo Decrypt");
	std::cout << "Decrypted : " << txt << std::endl;

	return 0;
}
