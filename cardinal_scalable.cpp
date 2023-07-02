#include "pch.h"
#include "cardinal_scalable.h"

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::bit_structure::bit_structure()
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		parts[i] = 0;
	}
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::bit_structure::operator [](unsigned int position) const noexcept
{
	return parts[position / PART_BIT_SIZE] & (1ull << (63 - (position & 63))); // position & 63 == position % 64
}

template<const unsigned int n, const unsigned int f>
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

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::bit_structure::flip() noexcept
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		parts[i] = ~parts[i];
	}
}

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

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(unsigned int value) noexcept
{
	bits.parts[n - 1] = value;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(long long value) noexcept
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
cardinal<n, f>::cardinal(unsigned long long value) noexcept
{
	bits.parts[n - 1] = value;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(float value) noexcept
{

}

template<const unsigned int n, const unsigned int f>
cardinal<n, f>::cardinal(double value) noexcept
{
	const unsigned long long	double_bits	= *(unsigned long long*)(&value);
	int							exponent	= ((double_bits & (0b11111111111ull << 52)) >> 52) - 1023;
	unsigned long long			mantissa	= (double_bits & ((1ull << 52) - 1)) + (1ull << 52);

	for (long long i = INTEGER_RIGHT + 52 - exponent; mantissa != 0 && i >= 0; i--)
	{
		if (i < (n + f) * PART_BIT_SIZE)
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



template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator - () const noexcept
{
	return inverted();
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator + (const cardinal<n, f>& other) const noexcept
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
void cardinal<n, f>::operator += (const cardinal<n, f>& other)
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
cardinal<n, f> cardinal<n, f>::operator - (const cardinal<n, f>& other) const noexcept
{
	return *this + other.inverted();
}

template<const unsigned int n, const unsigned int f>
void cardinal<n, f>::operator -= (const cardinal<n, f>& other)
{
	*this += other.inverted();
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator * (cardinal<n, f> other) const
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
	return result;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator / (const cardinal<n, f>& other) const
{
	cardinal<n, f> result, left, right, temp;
	left.set_to_max(true);
	right.set_to_max();
	temp = result * other;

	while (temp != *this)
	{
		if (temp > *this)
		{
			right = result;
		}
		else
		{
			left = result;
		}
		result = (left + right) >> 1;
		temp = result * other;
	}

	return result;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator << (int shift) const noexcept
{
	cardinal result(*this);
	result <<= shift;
	return result;
}

template<const unsigned int n, const unsigned int f>
cardinal<n, f> cardinal<n, f>::operator >> (int shift) const noexcept
{
	cardinal result(*this);
	result >>= shift;
	return result;
}


template<const unsigned int n, const unsigned int f>
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

template<const unsigned int n, const unsigned int f>
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

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator == (const cardinal<n, f>& other) const
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		if (bits[i] != other.bits[i])
		{
			return false;
		}
	}
	return true;
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator != (const cardinal<n, f>& other) const
{
	for (unsigned int i = 0; i < n + f; i++)
	{
		if (bits[i] != other.bits[i])
		{
			return true;
		}
	}
	return false;
}

template<const unsigned int n, const unsigned int f>
bool cardinal<n, f>::operator > (const cardinal<n, f>& other) const
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
bool cardinal<n, f>::operator >= (const cardinal<n, f>& other) const
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
bool cardinal<n, f>::operator < (const cardinal<n, f>& other) const
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
bool cardinal<n, f>::operator <= (const cardinal<n, f>& other) const
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

template<const unsigned int n, const unsigned int f>
std::string cardinal<n, f>::to_binary(bool add_spaces) const noexcept
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