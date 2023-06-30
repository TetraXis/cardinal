#include <string>
#include <iostream>

namespace experimental
{

#define PART_BIT_SIZE		64
#define SIGN				0
#define ULL_MAX_VALUE		18446744073709551615

	/*template <typename T>
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
	}*/

	/*template <typename T>
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
	}*/

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

		/*cardinal(const cardinal& value) noexcept
			: bits(value.bits)
		{}*/

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

		/*cardinal(const unsigned long long& left, const unsigned long long& middle, const unsigned long long& right, const unsigned long long& fractional)
			: bits(left, middle, right, fractional)
		{}*/



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

		/*cardinal(const double& value) noexcept
		{
			unsigned long long double_bits = *(unsigned long long*)(&value);
			int exponent = ((double_bits & (0b11111111111ull << 52)) >> 52) - 1023;
			unsigned long long mantissa = (double_bits & ((1ull << 52) - 1)) + (1ull << 52);
			for (int i = INTEGER_RIGHT + 52 - exponent; mantissa != 0 && i >= 0; i--)
			{
				if (i < BIT_SIZE)
				{
					bits.set_at(i, mantissa & 1);
				}
				mantissa >>= 1;
			}
			if (double_bits & 1ull << 63)
			{
				invert();
			}
		}*/

		cardinal(const std::string& value) noexcept
		{
			if (value[0] == '0' && value[1] == 'b') /* binary literal */
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
			/*
			bool fractional_overflow, right_overflow, middle_overflow;

			fractional_overflow = ULL_MAX_VALUE - result.bits.fractional < other.bits.fractional;
			result.bits.fractional += other.bits.fractional;

			right_overflow = ULL_MAX_VALUE - result.bits.right - fractional_overflow < other.bits.right || fractional_overflow && ULL_MAX_VALUE - result.bits.right - 1 == ULL_MAX_VALUE;
			result.bits.right += other.bits.right + fractional_overflow;

			middle_overflow = ULL_MAX_VALUE - result.bits.middle - right_overflow < other.bits.middle || right_overflow && ULL_MAX_VALUE - result.bits.middle - 1 == ULL_MAX_VALUE;
			result.bits.middle += other.bits.middle + right_overflow;

			result.bits.left += other.bits.left + middle_overflow;

			return result;*/
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

		/*
		cardinal	operator	*	(cardinal other) const
		{
			cardinal<n * 2, f> result;
			cardinal<n, f> self(*this);
			bool self_flip = self.bits[SIGN], other_flip = other.bits[SIGN];
			unsigned int self_uints[(n + f) * 2], other_uints[(n + f) * 2];

			if (self_flip)
			{
				self.invert();
			}
			if (other_flip)
			{
				other.invert();
			}

			for (int i = 0; i < n + f; i++)
			{
				self_uints[i*2] = (self.bits.parts[i] & 0b1111'1111'1111'1111'1111'1111'1111'1111'0000'0000'0000'0000'0000'0000'0000'0000) >> 32;
				self_uints[i*2 + 1] = self.bits.parts[i] & 0b1111'1111'1111'1111'1111'1111'1111'1111;
				other_uints[i*2] = (other.bits.parts[i] & 0b1111'1111'1111'1111'1111'1111'1111'1111'0000'0000'0000'0000'0000'0000'0000'0000) >> 32;
				other_uints[i*2 + 1] = other.bits.parts[i] & 0b1111'1111'1111'1111'1111'1111'1111'1111;
			}

			for (int i = (n + f) * 2 - 1; i > 0; i--)
			{
				for (int j = (n + f) * 2 - 1; j > 0; j--)
				{
					if (((n + f) * 4 - i - j - 2) * PART_BIT_SIZE / 2 - f * 2 * PART_BIT_SIZE < 0)
					{
						continue;
					}
					result += cardinal<n * 2, f>((unsigned long long)(self_uints[i]) * other_uints[j]) << ((n + f) * 4 - i - j - 2) * PART_BIT_SIZE / 2 - f * 2 * PART_BIT_SIZE;
				}
			}

			cardinal<n, f> final_result;

			for (int i = 0; i < n; i++) // looking for overflow
			{
				if (result.bits.parts[i] != 0)
				{
					for (int j = 0; j < n + f; j++)
					{
						final_result.bits.parts[j] = ULL_MAX_VALUE;
					}
					final_result.bits.set_at(SIGN, false);
					if (self_flip == other_flip)
					{
						return final_result;
					}
					return final_result.inverted();
				}
			}

			for (int i = 0; i < n + f; i++)
			{
				final_result.bits.parts[i] = result.bits.parts[n + i];
			}

			if (self_flip == other_flip)
			{
				return final_result;
			}
			return final_result.inverted();
		}
		*/



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

			/*std::string result = (bits[SIGN] ? "1" : "0");
			if (add_spaces)
			{
				result += " ";
			}
			for (int i = 1; i < n * PART_BIT_SIZE; i++)
			{
				result += bits[i] + '0';
			}
			if (add_spaces)
			{
				result += " ";
			}
			for (int i = n * PART_BIT_SIZE; i <= (n + f) * PART_BIT_SIZE; i++)
			{
				result += bits[i] + '0';
			}
			return result;*/
		}

		/*inline void increment() noexcept // add smallest amount
		{
			*this += {0, 0, 0, 1};
		}

		inline void decrement() noexcept // substruct smallest amount
		{
			*this += {18446744073709551615, 18446744073709551615, 18446744073709551615, 18446744073709551615};
		}*/

		void invert() /* changes sign */
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

		/*const cardinal as_integer() const
		{
			return { bits.left, bits.middle, bits.right, 0 };
		}*/

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
	}

}