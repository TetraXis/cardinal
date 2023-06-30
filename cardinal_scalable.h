#pragma once
#include <string>
#include <iostream>

#define PART_BIT_SIZE		64
#define SIGN				0
#define ULL_MAX_VALUE		18446744073709551615

template <typename T>
std::string as_binary(T value)
{
	const unsigned int size = sizeof(T) * 8;
	unsigned long long new_value = *(unsigned long long*)(&value);
	std::string result = "";
	for (unsigned int i = 0; i < size; i++)
	{
		result += (new_value & 1) + '0';
		new_value >>= 1;
	}
	for (unsigned int i = 0; i < size / 2; i++)
	{
		std::swap(result[i], result[size - i - 1]);
	}
	return result;
}

template <typename T>
std::wstring as_wide_binary(T value)
{
	const unsigned int size = sizeof(T) * 8;
	unsigned long long new_value = *(unsigned long long*)(&value);
	std::wstring result = L"";
	for (unsigned int i = 0; i < size; i++)
	{
		if (i % 32 == 0)
		{
			result += '\n';
		}
		result += (new_value & 1) + '0';
		new_value >>= 1;
	}
	for (unsigned int i = 0; i < size / 2; i++)
	{
		std::swap(result[i], result[size - i - 1]);
	}
	return result;
}

template<unsigned int n = 3, unsigned int f = 1>
struct cardinal
{
	struct bit_structure
	{
		unsigned long long parts[n + f];

		bit_structure();
		bool operator [](unsigned int position) const noexcept;
		void set_at(const unsigned int& position, const bool& value) noexcept;
		void flip() noexcept;
	} bits;

	cardinal() {}
	cardinal(cardinal const&) = default;
	cardinal(cardinal<n / 2, f> other);
	cardinal(cardinal<n * 2, f> other);
	cardinal(const bool& value);
	cardinal(int value) noexcept;
	cardinal(const unsigned int& value) noexcept;
	cardinal(const long long& value) noexcept;
	cardinal(const unsigned long long& value) noexcept;
	cardinal(const std::string& value) noexcept;

	cardinal<n, f>	operator	-	() const noexcept;
	cardinal<n, f>	operator	+	(const cardinal<n, f>& other) const noexcept;
	void			operator	+=	(const cardinal<n, f>& other);
	cardinal<n, f>	operator	-	(const cardinal<n, f>& other) const noexcept;
	cardinal<n, f>	operator	*	(cardinal<n, f> other) const;
	cardinal<n, f>	operator	<<	(int shift) const noexcept;
	cardinal<n, f>	operator	>>	(int shift) const noexcept;

	void			operator	<<=	(int shift) noexcept;
	void			operator	>>=	(int shift) noexcept;

	bool			operator	==	(const cardinal<n, f>& other) const;

	std::string		to_binary(const bool& add_spaces = false) const noexcept;
	void			invert();
	cardinal<n, f>	inverted() const;
	const bool		get_bit(unsigned int position) const;
	void			set_bit(unsigned int position, bool value = false);
	void			set_to_max(bool sign = false);
};

template<unsigned int n, unsigned int f>
std::wstring ToString(const cardinal<n, f>& q)
{
	return as_wide_binary(q);
}