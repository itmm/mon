
#line 4 "index.md"

	
#line 14 "index.md"

	#include <iostream>

#line 219 "index.md"

	#include <termios.h>
	#include <unistd.h>

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

	
#line 81 "index.md"

	
#line 90 "index.md"

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

#line 82 "index.md"

	static void write_hex_byte(int byte) {
		
#line 110 "index.md"

	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		put("??");
	}

#line 84 "index.md"

	}

#line 46 "index.md"

	static void write_addr(
		const char *addr
	) {
		
#line 121 "index.md"

	unsigned long value {
		reinterpret_cast<unsigned long>(
			addr
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

#line 137 "index.md"

	void dump_hex(const char *from,
		const char *to
	) {
		
#line 158 "index.md"

	put("\x1b[0E\x1b[2K");
	constexpr int bytes_per_row { 16 };
	if (from && from < to) {
		for (
			; from < to;
			from += bytes_per_row
		) {
			write_addr(from);
			put(": ");
			
#line 178 "index.md"
 {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		if (from + row < to) {
			write_hex_byte(
				from[row] & 0xff
			);
		} else {
			put("  ");
		}
		put(' ');
		
#line 211 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 189 "index.md"

	}
} 
#line 168 "index.md"

			put("| ");
			
#line 195 "index.md"
 {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		if (from + row >= to) {
			put(' ');
		} else if (isprint(from[row])) {
			put(from[row]);
		} else {
			put('.');
		}
		
#line 211 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 205 "index.md"

	}
} 
#line 170 "index.md"

			put('\n');
		}
	}

#line 141 "index.md"

	}

#line 226 "index.md"

	class Term_Handler {
		termios orig_;
	public:
		Term_Handler() {
			tcgetattr(STDIN_FILENO, &orig_);
			termios raw { orig_ };
			raw.c_lflag &= ~(ECHO | ICANON);
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
		}
		~Term_Handler() {
			tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_);
		}
	};

#line 6 "index.md"

	int main() {
		
#line 56 "index.md"

	
#line 244 "index.md"
 
	Term_Handler term_handler;

#line 57 "index.md"

	char buffer[8 * 1024];
	char *addr { reinterpret_cast<char *>(
		buffer
	) };
	for (;;) {
		write_addr(addr);
		put("> ");
		int cmd { get() };
		if (cmd == 0x04 || cmd == EOF) { 
			put("quit\n");
			break;
		}
		
#line 147 "index.md"

	if (cmd == '\n' || cmd == '\r') {
		char *from { addr };
		addr += 8 * 16;
		dump_hex(from, addr);
		continue;
	}

#line 70 "index.md"

		put("unknown command ");
		put(isprint(cmd) ? (char) cmd : '?');
		put(" (");
		write_hex_byte(cmd & 0xff);
		put(")\n");
	}

#line 8 "index.md"

	}
