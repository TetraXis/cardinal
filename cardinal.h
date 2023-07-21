#pragma once
#include <string>
#include <iostream>

#define PART_BIT_SIZE		64
#define SIGN				0
#define ULL_MAX_VALUE		18446744073709551615

template<const unsigned int n = 3, const unsigned int f = 1>
struct cardinal
{
	struct bit_structure
	{
		unsigned long long parts[n + f]{};

		bit_structure();

		bool operator []		(unsigned int position) const;
		void set_at				(const unsigned int& position, const bool& value);
		unsigned long long popcount	() const;
		void flip				();
	} bits;

	cardinal();
	cardinal(cardinal<n, f> const&);
	cardinal(cardinal<n / 2, f> other);
	cardinal(cardinal<n * 2, f> other);
	cardinal(bool value);
	cardinal(int value);
	cardinal(unsigned int value);
	cardinal(long long value);
	cardinal(unsigned long long value);
	cardinal(float value);
	cardinal(double value);
	cardinal(const std::string& value);

	static cardinal<n, f> midpoint(const cardinal<n, f>& a, const cardinal<n, f>& b);
	static cardinal<n, f> mult_clamp(const cardinal<n, f>& a, const cardinal<n, f>& b);


	cardinal<n, f>	operator	-	()								const;
	cardinal<n, f>	operator	~	()								const;

	cardinal<n, f>&	operator	++	();		// prefix
	cardinal<n, f>	operator	++	(int);	// postfix
	cardinal<n, f>&	operator	--	();		// prefix
	cardinal<n, f>	operator	--	(int);	// postfix

	cardinal<n, f>	operator	+	(const cardinal<n, f>& other)	const;
	cardinal<n, f>	operator	-	(const cardinal<n, f>& other)	const;
	cardinal<n, f>	operator	*	(cardinal<n, f> other)			const;
	cardinal<n, f>	operator	/	(cardinal<n, f> other)			const;
	cardinal<n, f>	operator	%	(cardinal<n, f> other)			const;
	void			operator	+=	(const cardinal<n, f>& other);
	void			operator	-=	(const cardinal<n, f>& other);
	void			operator	*=	(const cardinal<n, f>& other);
	void			operator	/=	(const cardinal<n, f>& other);
	void			operator	%=	(const cardinal<n, f>& other);

	bool			operator	==	(const cardinal<n, f>& other)	const;
	bool			operator	!=	(const cardinal<n, f>& other)	const;
	bool			operator	>	(const cardinal<n, f>& other)	const;
	bool			operator	>=	(const cardinal<n, f>& other)	const;
	bool			operator	<	(const cardinal<n, f>& other)	const;
	bool			operator	<=	(const cardinal<n, f>& other)	const;

	cardinal<n, f>	operator	<<	(int shift)						const;
	cardinal<n, f>	operator	>>	(int shift)						const;
	void			operator	<<=	(int shift);
	void			operator	>>=	(int shift);

	cardinal<n, f>	operator	&	(const cardinal<n, f>& other)	const;
	cardinal<n, f>	operator	|	(const cardinal<n, f>& other)	const;
	cardinal<n, f>	operator	^	(const cardinal<n, f>& other)	const;
	void			operator	&=	(const cardinal<n, f>& other);
	void			operator	|=	(const cardinal<n, f>& other);
	void			operator	^=	(const cardinal<n, f>& other);


	void			invert();
	cardinal<n, f>	inverted()			const;
	void			add_smallest();
	void			sub_smallest();
	cardinal<n, f>	added_smallest()	const;
	cardinal<n, f>	subed_smallest()	const;
	void			set_to_max(bool sign = false);

	const bool		get_bit(unsigned int position)	const;
	void			set_bit(unsigned int position, bool value);


	std::string		to_binary(bool add_spaces = false)	const;
	std::string		to_string()	const;
};

template <typename T>
std::string as_binary(T value);

template <typename T>
std::wstring as_wide_binary(T value);

template<unsigned int n, unsigned int f>
std::wstring ToString(const cardinal<n, f>& q);


//========== IMPLEMENTATION ==========//


template<typename T>
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

template<typename T>
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

template<unsigned int n, unsigned int f>
std::wstring ToString(const cardinal<n, f>& q)
{
	return as_wide_binary(q);
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::bit_structure::bit_structure()
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		parts[i] = 0;
	}
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::bit_structure::operator [](unsigned int position) const
{
	return parts[position / PART_BIT_SIZE] & (1ull << (63 - (position & 63))); // position & 63 == position % 64
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::bit_structure::set_at(const unsigned int& position, const bool& value)
{
	if (value)
	{
		parts[position / PART_BIT_SIZE] |= (1ull << (63 - (position & 63)));
		return;
	}
	parts[position / PART_BIT_SIZE] &= ~(1ull << (63 - (position & 63)));
	return;
}

template<unsigned int n, unsigned int f>
unsigned long long cardinal<n, f>::bit_structure::popcount() const
{
	unsigned long long result = 0;
	for (unsigned int i = 0; i < n + f; i++)
	{
		result += __popcnt64(parts[i]);
	}
	return result;
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::bit_structure::flip()
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		parts[i] = ~parts[i];
	}
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal() {}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(cardinal<n, f> const&) = default;

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(cardinal<n / 2, f> other)
{
	bool flip = other.bits[SIGN];
	if (flip)
	{
		other.invert();
	}
	for (int i = n + f - 1, diff = (n + 1) / 2; i >= diff; i--)
	{
		bits.parts[i] = other.bits.parts[i - diff];
	}
	if (flip)
	{
		invert();
	}
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(cardinal<n * 2, f> other)
{
	bool flip = other.bits[SIGN];
	if (flip)
	{
		other.invert();
	}
	for (int i = 0; i < n; i++) // looking for overflow
	{
		if (other.bits.parts[i] != 0)
		{
			//cout << "OVERFLOW\n";
			set_to_max(flip);
			return;
		}
	}
	if (other.bits.parts[n] & 1ull << 63)
	{
		set_to_max(flip);
		return;
	}
	for (int i = 0; i < n + f; i++)
	{
		bits.parts[i] = other.bits.parts[n + i];
	}
	if (flip)
	{
		invert();
	}
}



template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(bool value)
{
	if (value)
	{
		bits.parts[n - 1] = 1;
	}
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(int value)
{
	if (value < 0)
	{
		bits.parts[n - 1] = -value;
		invert();
		return;
	}
	bits.parts[n - 1] = value;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(unsigned int value)
{
	bits.parts[n - 1] = value;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(long long value)
{
	if (value < 0)
	{
		bits.parts[n - 1] = -value;
		invert();
		return;
	}
	bits.parts[n - 1] = value;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(unsigned long long value)
{
	bits.parts[n - 1] = value;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(float value)
{
	// UNDONE: cardinal(float)
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(double value)
{
	const unsigned long long	double_bits = *(unsigned long long*)(&value);
	int							exponent = ((double_bits & (0b11111111111ull << 52)) >> 52) - 1023;
	unsigned long long			mantissa = (double_bits & ((1ull << 52) - 1)) + (1ull << 52);

	for (unsigned int i = n * PART_BIT_SIZE - 1 + 52 - exponent; mantissa != 0 && i >= 0; i--)
	{
		if (i < (n + f)* PART_BIT_SIZE)
		{
			bits.set_at(i, mantissa & 1);
		}
		mantissa >>= 1;
	}
	if (double_bits & 1ull << 63)
	{
		invert();
	}
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(const std::string & value)
{
	if (value[0] == '0' && value[1] == 'b')
	{
		for (unsigned int i = value.size() - 1, j = (n + f) * PART_BIT_SIZE - 1; i > 1; --i)
		{
			if (value[i] != '0' && value[i] != '1')
			{
				continue;
			}
			bits.set_at(j, value[i] == '1');
			j--;
		}
		return;
	}
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator - () const
{
	return inverted();
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator~() const
{
	// UNDONE: ~
	return cardinal<n, f>();
}

template<unsigned int n, unsigned int f>
cardinal<n, f>& cardinal<n, f>::operator++()
{
	// UNDONE: ++
	return this;
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator++(int)
{
	// UNDONE: ++(int)
	return cardinal<n, f>();
}

template<unsigned int n, unsigned int f>
cardinal<n, f>& cardinal<n, f>::operator--()
{
	// UNDONE: --
	return this;
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator--(int)
{
	// UNDONE: --(int)
	return cardinal<n, f>();
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator + (const cardinal<n, f>&other) const
{
	cardinal result(*this);
	bool overflow_p = false, overflow = false;

	for (int i = n + f - 1; i >= 0; i--)
	{
		overflow = ULL_MAX_VALUE - result.bits.parts[i] - overflow_p < other.bits.parts[i] || overflow_p && ULL_MAX_VALUE - result.bits.parts[i] - 1 == ULL_MAX_VALUE;
		result.bits.parts[i] += other.bits.parts[i] + overflow_p;
		overflow_p = overflow;
	}

	return result;
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::operator += (const cardinal<n, f>&other)
{
	bool overflow_p = false, overflow = false;

	for (int i = n + f - 1; i >= 0; i--)
	{
		overflow = ULL_MAX_VALUE - bits.parts[i] - overflow_p < other.bits.parts[i] || overflow_p && ULL_MAX_VALUE - bits.parts[i] - 1 == ULL_MAX_VALUE;
		bits.parts[i] += other.bits.parts[i] + overflow_p;
		overflow_p = overflow;
	}
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator - (const cardinal<n, f>&other) const
{
	return *this + other.inverted();
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::operator -= (const cardinal<n, f>&other)
{
	*this += other.inverted();
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator*=(const cardinal<n, f>& other)
{
	// UNDONE: *=
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator/=(const cardinal<n, f>& other)
{
	// UNDONE: /=
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator%=(const cardinal<n, f>& other)
{
	// UNDONE: %=
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator * (cardinal<n, f> other) const
{
	cardinal<n, f> result;
	int shift;

	bool flip = other.bits.popcount() > ((n + f) * PART_BIT_SIZE >> 1);
	if (flip)
	{
		other.invert();
	}

	for (int i = n + f - 1; i >= 0; i--)
	{
		shift = (n - f - i) * PART_BIT_SIZE;
		while (other.bits.parts[i] > 0)
		{
			if (other.bits.parts[i] & 1)
			{
				result += *this << shift;
			}
			shift++;
			other.bits.parts[i] >>= 1;
		}
	}

	if (flip)
	{
		return result.inverted();
	}
	return result;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator / (cardinal<n, f> other) const
{
	bool flip = other.bits.parts[0] & (1ull << 63);
	if (flip)
	{
		other.invert();
	}

	cardinal<n, f> result, left, right, temp;
	right.set_to_max();
	left = right.inverted(); // cannot use set_max(true)
	temp = mult_clamp(result, other);

	while (temp != *this && left.added_smallest() != right)
	{
		if (temp > *this)
		{
			right = result;
		}
		else
		{
			left = result;
		}
		result = midpoint(left, right);

		temp = mult_clamp(result, other);
	}

	if (flip)
	{
		result.invert();
	}
	return result;
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator%(cardinal<n, f> other) const
{
	// UNDONE: %
	return cardinal<n, f>();
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator << (int shift) const
{
	cardinal result(*this);
	result <<= shift;
	return result;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator >> (int shift) const
{
	cardinal result(*this);
	result >>= shift;
	return result;
}


template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::operator <<= (int shift)
{
	if (shift < 0)
	{
		*this >>= (-shift);
		return;
	}
	for (int i = 0; i < (n + f) * PART_BIT_SIZE; ++i)
	{
		if (i + shift < (n + f) * PART_BIT_SIZE)
		{
			bits.set_at(i, bits[i + shift]);
		}
		else
		{
			bits.set_at(i, false);
		}
	}
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::operator >>= (int shift)
{
	if (shift < 0)
	{
		*this <<= (-shift);
		return;
	}
	bool sign = bits[SIGN];
	for (int i = (n + f) * PART_BIT_SIZE - 1; i >= 0; --i)
	{
		if (i - shift >= 0)
		{
			bits.set_at(i, bits[i - shift]);
		}
		else
		{
			bits.set_at(i, sign);
		}
	}
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator&(const cardinal<n, f>& other) const
{
	// UNDONE: &
	return cardinal<n, f>();
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator|(const cardinal<n, f>& other) const
{
	// UNDONE: |
	return cardinal<n, f>();
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator^(const cardinal<n, f>& other) const
{
	// UNDONE: ^
	return cardinal<n, f>();
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator&=(const cardinal<n, f>& other)
{
	// UNDONE: &=
	return cardinal<n, f>();
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator|=(const cardinal<n, f>& other)
{
	// UNDONE: |=
	return cardinal<n, f>();
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator^=(const cardinal<n, f>& other)
{
	// UNDONE: ^=
	return cardinal<n, f>();
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator == (const cardinal<n, f>&other) const
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		if (bits.parts[i] != other.bits.parts[i])
		{
			return false;
		}
	}
	return true;
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator != (const cardinal<n, f>&other) const
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		if (bits.parts[i] != other.bits.parts[i])
		{
			return true;
		}
	}
	return false;
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator > (const cardinal<n, f>&other) const
{
	if (bits[SIGN] != other.bits[SIGN])
	{
		return other.bits[SIGN];
	}
	for (unsigned int i = 0; i < n + f; i++)
	{
		if (bits.parts[i] != other.bits.parts[i])
		{
			return bits.parts[i] > other.bits.parts[i];
		}
	}
	return false;
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator >= (const cardinal<n, f>&other) const
{
	if (bits[SIGN] != other.bits[SIGN])
	{
		return other.bits[SIGN];
	}
	for (unsigned int i = 0; i < n + f; i++)
	{
		if (bits.parts[i] != other.bits.parts[i])
		{
			return bits.parts[i] > other.bits.parts[i];
		}
	}
	return true;
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator < (const cardinal<n, f>&other) const
{
	if (bits[SIGN] != other.bits[SIGN])
	{
		return bits[SIGN];
	}
	for (unsigned int i = 0; i < n + f; i++)
	{
		if (bits.parts[i] != other.bits.parts[i])
		{
			return bits.parts[i] < other.bits.parts[i];
		}
	}
	return false;
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator <= (const cardinal<n, f>&other) const
{
	if (bits[SIGN] != other.bits[SIGN])
	{
		return bits[SIGN];
	}
	for (unsigned int i = 0; i < n + f; i++)
	{
		if (bits.parts[i] != other.bits.parts[i])
		{
			return bits.parts[i] < other.bits.parts[i];
		}
	}
	return true;
}

template<unsigned int n, unsigned int f>
inline cardinal<n, f> cardinal<n, f>::midpoint(const cardinal<n, f>& a, const cardinal<n, f>& b)
{
	//cardinal result(a);
	/*bool overflow_p = false, overflow = false;

	for (int i = n + f - 1; i >= 0; i--)
	{
		overflow = ULL_MAX_VALUE - result.bits.parts[i] - overflow_p < b.bits.parts[i] || overflow_p && ULL_MAX_VALUE - result.bits.parts[i] - 1 == ULL_MAX_VALUE;
		result.bits.parts[i] += b.bits.parts[i] + overflow_p;
		overflow_p = overflow;
	}
	result >>= 1;
	result.set_bit(0, overflow); // <- is incorrect*/
	return a + ((b - a) >> 1);
}

template<unsigned int n, unsigned int f>
inline cardinal<n, f> cardinal<n, f>::mult_clamp(const cardinal<n, f>& a, const cardinal<n, f>& b)
{
	return cardinal< n << 1, f>(a) * cardinal< n << 1, f>(b);
}

template<const unsigned int n, const unsigned int f>
std::string cardinal<n, f>::to_binary(bool add_spaces) const
{
	cardinal self = *this;
	std::string result = "";
	for (int i = n + f - 1; i >= 0; i--)
	{
		for (int j = 0; j < PART_BIT_SIZE; j++)
		{
			result += '0' + self.bits.parts[i] % 2;
			self.bits.parts[i] /= 2;
		}
		if (add_spaces)
		{
			result += ' ';
		}
	}
	for (unsigned int i = 0; i < result.size() / 2; i++)
	{
		std::swap(result[i], result[result.size() - 1 - i]);
	}
	return result;
}

template<unsigned int n, unsigned int f>
std::string cardinal<n, f>::to_string() const
{
	// UNDONE: to_string
	return std::string();
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::invert()
{
	cardinal one;
	one.bits.parts[n + f - 1] = 1;
	if (bits.parts[SIGN])
	{
		*this = *this + one.inverted();
		bits.flip();
	}
	else
	{
		bits.flip();
		*this = *this + one;
	}
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::inverted() const
{
	cardinal result(*this);
	result.invert();
	return result;
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::add_smallest()
{
	cardinal<n, f> one;
	one.bits.parts[n + f - 1] = 1;
	*this += one;
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::sub_smallest()
{
	cardinal<n, f> one;
	one.bits.parts[n + f - 1] = 1;
	*this += one.inverted;
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::added_smallest() const
{
	cardinal<n, f> self(*this);
	self.add_smallest();
	return self;
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::subed_smallest() const
{
	cardinal<n, f> self(*this);
	self.sub_smallest();
	return self;
}

template<const unsigned int n, const unsigned int f>
const bool cardinal<n, f>::get_bit(unsigned int position) const
{
	return bits[position];
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::set_bit(unsigned int position, bool value)
{
	bits.set_at(position, value);
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::set_to_max(bool sign)
{
	if (sign)
	{
		bits.parts[0] = 1ull << 63;
		for (int i = 1; i < n + f; i++)
		{
			bits.parts[i] = 0;
		}
		return;
	}
	bits.parts[0] = (1ull << 63) - 1ull;
	for (int i = 1; i < n + f; i++)
	{
		bits.parts[i] = ULL_MAX_VALUE;
	}
}