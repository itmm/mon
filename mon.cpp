
#line 4 "index.md"

	
#line 14 "index.md"

	#include <iostream>

#line 5 "index.md"

	
#line 20 "index.md"

	
#line 45 "index.md"

	
#line 54 "index.md"

	static void write_hex_nibble(int nibble) {
		static const char digits[] = "0123456789abcdef";
		static_assert(sizeof(digits) == 16 + 1);
		if (nibble >= 0 && nibble <= 15) {
			std::cout << digits[nibble];
		} else {
			std::cout << '?';
		}
	}

#line 46 "index.md"

	static void write_hex_byte(int byte) {
		
#line 68 "index.md"

	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		std::cout << "??";
	}

#line 48 "index.md"

	}

#line 21 "index.md"

	static void write_addr(
		const char *addr
	) {
		
#line 79 "index.md"

	unsigned long value { reinterpret_cast<unsigned long>(addr) };
	std::cout << '$';
	int shift { (sizeof(long) - 1) * 8 };
	for (; shift >= 0; shift -= 8) {
		write_hex_byte((value >> shift) & 0xff);
	}

#line 25 "index.md"

	}

#line 6 "index.md"

	int main() {
		
#line 31 "index.md"

	char *addr {
		reinterpret_cast<char *>(write_addr)
	};
	for (;;) {
		write_addr(addr);
		std::cout << "> ";
		std::cout << "unknown command\n";
		break;
	}

#line 8 "index.md"

	}
