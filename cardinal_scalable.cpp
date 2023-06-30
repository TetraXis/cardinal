#include "pch.h"
#include "cardinal_scalable.h"

template<unsigned int n, unsigned int f>
cardinal<n, f>::bit_structure::bit_structure()
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		parts[i] = 0;
	}
}

template<unsigned int n, unsigned int f>
bool cardinal<n, f>::bit_structure::operator [](unsigned int position) const noexcept
{
	return parts[position / PART_BIT_SIZE] & (1ull << (63 - (position & 63))); // position & 63 == position % 64
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::bit_structure::set_at(const unsigned int& position, const bool& value) noexcept
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
void cardinal<n, f>::bit_structure::flip() noexcept
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		parts[i] = ~parts[i];
	}
}

template<unsigned int n, unsigned int f>
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

template<unsigned int n, unsigned int f>
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



template<unsigned int n, unsigned int f>
cardinal<n, f>::cardinal(const bool& value)
{
	if (value)
	{
		bits.parts[n - 1] = 1;
	}
}

template<unsigned int n, unsigned int f>
cardinal<n, f>::cardinal(int value) noexcept
{
	if (value < 0)
	{
		bits.parts[n - 1] = -value;
		invert();
		return;
	}
	bits.parts[n - 1] = value;
}

template<unsigned int n, unsigned int f>
cardinal<n, f>::cardinal(const unsigned int& value) noexcept
{
	bits.parts[n - 1] = value;
}

template<unsigned int n, unsigned int f>
cardinal<n, f>::cardinal(const long long& value) noexcept
{
	if (value < 0)
	{
		bits.parts[n - 1] = -value;
		invert();
		return;
	}
	bits.parts[n - 1] = value;
}

template<unsigned int n, unsigned int f>
cardinal<n, f>::cardinal(const unsigned long long& value) noexcept
{
	bits.parts[n - 1] = value;
}

template<unsigned int n, unsigned int f>
cardinal<n, f>::cardinal(const std::string& value) noexcept
{
	if (value[0] == '0' && value[1] == 'b')
	{
		for (unsigned int i = value.size() - 1, j = BIT_SIZE - 1; i > 1; --i)
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



template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator - () const noexcept
{
	return inverted();
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator + (const cardinal& other) const noexcept
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

template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator += (const cardinal& other)
{
	bool overflow_p = false, overflow = false;

	for (int i = n + f - 1; i >= 0; i--)
	{
		overflow = ULL_MAX_VALUE - bits.parts[i] - overflow_p < other.bits.parts[i] || overflow_p && ULL_MAX_VALUE - bits.parts[i] - 1 == ULL_MAX_VALUE;
		bits.parts[i] += other.bits.parts[i] + overflow_p;
		overflow_p = overflow;
	}
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator - (const cardinal& other) const noexcept
{
	return *this + other.inverted();
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator * (cardinal other) const
{
	cardinal<n << 1, f> result;
	int shift;
	bool flip_other = other.bits[SIGN];
	if (flip_other)
	{
		other.invert();
	}

	for (int i = n + f - 1; i > 0; i--)
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
	if (flip_other)
	{
		result.invert();
	}
	// cout << result.to_binary() << " big\n";
	return result;
}



template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator << (int shift) const noexcept
{
	cardinal result(*this);
	result <<= shift;
	return result;
}

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::operator >> (int shift) const noexcept
{
	cardinal result(*this);
	result >>= shift;
	return result;
}


template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator <<= (int shift) noexcept
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

template<unsigned int n, unsigned int f>
void cardinal<n, f>::operator >>= (int shift) noexcept
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
bool cardinal<n, f>::operator == (const cardinal<n, f>& other) const
{
	for (int i = 0; i < n + f; i++)
	{
		if (bits[i] != other.bits[i])
		{
			return false;
		}
	}
	return true;
}


template<unsigned int n, unsigned int f>
std::string cardinal<n, f>::to_binary(const bool& add_spaces) const noexcept
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
	}
	for (unsigned int i = 0; i < result.size() / 2; i++)
	{
		std::swap(result[i], result[result.size() - 1 - i]);
	}
	return result;
	return "A";
}

template<unsigned int n, unsigned int f>
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

template<unsigned int n, unsigned int f>
cardinal<n, f> cardinal<n, f>::inverted() const
{
	cardinal result(*this);
	result.invert();
	return result;
}

template<unsigned int n, unsigned int f>
const bool cardinal<n, f>::get_bit(unsigned int position) const
{
	return bits[position];
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::set_bit(unsigned int position, bool value)
{
	bits.set_at(position, value);
}

template<unsigned int n, unsigned int f>
void cardinal<n, f>::set_to_max(bool sign)
{
	bits.parts[0] = ULL_MAX_VALUE >> 1;
	for (int i = 1; i < n + f; i++)
	{
		bits.parts[i] = ULL_MAX_VALUE;
	}
	if (sign)
	{
		invert();
	}
}

/*
template<unsigned int n = 3, unsigned int f = 1>
struct cardinal
{
	struct bit_structure
	{
		unsigned long long parts[n + f];

		bit_structure()
		{
			for (unsigned int i = 0; i < n + f; i++)
			{
				parts[i] = 0;
			}
		}

		const bool operator [](unsigned int position) const noexcept
		{
			return parts[position / PART_BIT_SIZE] & (1ull << (63 - (position & 63))); // position & 63 == position % 64
		}

		void set_at(const unsigned int& position, const bool& value) noexcept
		{
			if (value)
			{
				parts[position / PART_BIT_SIZE] |= (1ull << (63 - (position & 63)));
				return;
			}
			parts[position / PART_BIT_SIZE] &= ~(1ull << (63 - (position & 63)));
			return;
		}

		inline void flip() noexcept
		{
			for (unsigned int i = 0; i < n + f; i++)
			{
				parts[i] = ~parts[i];
			}
		}
	} bits;

	cardinal()
	{}

	cardinal(cardinal const&) = default;

	cardinal(cardinal<n / 2, f> other)
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

	cardinal(cardinal<n * 2, f> other)
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



	cardinal(const bool& value)
	{
		if (value)
		{
			bits.parts[n - 1] = 1;
		}
	}

	cardinal(int value) noexcept
	{
		if (value < 0)
		{
			bits.parts[n - 1] = -value;
			invert();
			return;
		}
		bits.parts[n - 1] = value;
	}

	cardinal(const unsigned int& value) noexcept
	{
		bits.parts[n - 1] = value;
	}

	cardinal(const long long& value) noexcept
	{
		if (value < 0)
		{
			bits.parts[n - 1] = -value;
			invert();
			return;
		}
		bits.parts[n - 1] = value;
	}

	cardinal(const unsigned long long& value) noexcept
	{
		bits.parts[n - 1] = value;
	}

	cardinal(const std::string& value) noexcept
	{
		if (value[0] == '0' && value[1] == 'b')
		{
			for (unsigned int i = value.size() - 1, j = BIT_SIZE - 1; i > 1; --i)
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



	cardinal	operator	-	() const noexcept
	{
		return inverted();
	}

	cardinal	operator	+	(const cardinal& other) const noexcept
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

	void		operator	+=	(const cardinal& other)
	{
		bool overflow_p = false, overflow = false;

		for (int i = n + f - 1; i >= 0; i--)
		{
			overflow = ULL_MAX_VALUE - bits.parts[i] - overflow_p < other.bits.parts[i] || overflow_p && ULL_MAX_VALUE - bits.parts[i] - 1 == ULL_MAX_VALUE;
			bits.parts[i] += other.bits.parts[i] + overflow_p;
			overflow_p = overflow;
		}
	}

	cardinal	operator	-	(const cardinal& other) const noexcept
	{
		return *this + other.inverted();
	}

	cardinal	operator	*	(cardinal other) const
	{
		cardinal<n << 1, f> result;
		int shift;
		bool flip_other = other.bits[SIGN];
		if (flip_other)
		{
			other.invert();
		}

		for (int i = n + f - 1; i > 0; i--)
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
		if (flip_other)
		{
			result.invert();
		}
		// cout << result.to_binary() << " big\n";
		return result;
	}



	cardinal	operator	<<	(int shift) const noexcept
	{
		cardinal result(*this);
		result <<= shift;
		return result;
	}

	cardinal	operator	>>	(int shift) const noexcept
	{
		cardinal result(*this);
		result >>= shift;
		return result;
	}


	void		operator	<<=	(int shift) noexcept
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

	void		operator	>>=	(int shift) noexcept
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

	bool		operator	==	(const cardinal<n, f>& other) const
	{
		for (int i = 0; i < n + f; i++)
		{
			if (bits[i] != other.bits[i])
			{
				return false;
			}
		}
		return true;
	}


	std::string to_binary(const bool& add_spaces = false) const noexcept
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
		}
		for (unsigned int i = 0; i < result.size() / 2; i++)
		{
			std::swap(result[i], result[result.size() - 1 - i]);
		}
		return result;
		return "A";
	}

	void invert() 
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

	const cardinal inverted() const
	{
		cardinal result(*this);
		result.invert();
		return result;
	}

	const bool get_bit(unsigned int position) const
	{
		return bits[position];
	}

	void set_bit(unsigned int position, bool value = false)
	{
		bits.set_at(position, value);
	}

	void set_to_max(bool sign = false)
	{
		bits.parts[0] = ULL_MAX_VALUE >> 1;
		for (int i = 1; i < n + f; i++)
		{
			bits.parts[i] = ULL_MAX_VALUE;
		}
		if (sign)
		{
			invert();
		}
	}
};

std::wstring ToString(const cardinal<>& q)
{
	return as_wide_binary(q);
}*/