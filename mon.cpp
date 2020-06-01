
#line 4 "index.md"

	
#line 14 "index.md"

	#include <iostream>

#line 5 "index.md"

	
#line 20 "index.md"

	void put(char ch) {
		std::cout.put(ch);
	}
	void put(const char *begin, const char *end) {
		if (begin && begin < end) {
			while (begin != end) {
				put(*begin++);
			}
		}
	}
	void put(const char *begin) {
		if (begin) {
			while (*begin) {
				put(*begin++);
			}
		}
	}
	int get() {
		return std::cin.get();
	}

#line 45 "index.md"

	
#line 75 "index.md"

	
#line 84 "index.md"

	static void write_hex_nibble(
		int nibble
	) {
		static const char digits[] {
			"0123456789abcdef"
		};
		static_assert(
			sizeof(digits) == 16 + 1
		);
		if (nibble >= 0 && nibble <= 15) {
			put(digits[nibble]);
		} else {
			put('?');
		}
	}

#line 76 "index.md"

	static void write_hex_byte(int byte) {
		
#line 104 "index.md"

	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		put("??");
	}

#line 78 "index.md"

	}

#line 46 "index.md"

	static void write_addr(
		const char *addr
	) {
		
#line 115 "index.md"

	char buffer[8 * 1024];
	unsigned long value {
		reinterpret_cast<unsigned long>(
			buffer
		)
	};
	int shift { (sizeof(long) - 1) * 8 };
	for (; shift >= 0; shift -= 8) {
		write_hex_byte(
			(value >> shift) & 0xff
		);
	}

#line 50 "index.md"

	}

#line 6 "index.md"

	int main() {
		
#line 56 "index.md"

	char *addr { reinterpret_cast<char *>(
		write_addr
	) };
	for (;;) {
		write_addr(addr);
		put("> ");
		int cmd { get() };
		if (cmd == EOF) { 
			put("quit\n");
			break;
		}
		
#line 132 "index.md"

	if (cmd == '\n' || cmd == '\r') {
		
#line 141 "index.md"

	for (
		int lines { 8 }; lines; --lines
	) {
		constexpr int bytes_per_row {
			16
		};
		write_addr(addr);
		put(": ");
		
#line 160 "index.md"
 {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		write_hex_byte(addr[row] & 0xff);
		put(' ');
		
#line 185 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 165 "index.md"

	}
} 
#line 150 "index.md"

		put("| ");
		
#line 171 "index.md"
 {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		if (isprint(addr[row])) {
			put(addr[row]);
		} else {
			put('.');
		}
		
#line 185 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 179 "index.md"

	}
} 
#line 152 "index.md"

		put('\n');
		addr += bytes_per_row;
	}

#line 134 "index.md"

		continue;
	}

#line 68 "index.md"

		put("unknown command\n");
	}

#line 8 "index.md"

	}
