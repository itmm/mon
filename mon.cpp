
#line 4 "index.md"

	
#line 14 "index.md"

	#include <iostream>

#line 234 "index.md"

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
	void putnl() {
		put('\n');
	}
	int get() {
		int res { std::cin.get() };
		if (res == 0x04) { res = EOF; }
		if (res == '\r') { res = '\n'; }
		return res;
	}

#line 51 "index.md"

	
#line 96 "index.md"

	
#line 105 "index.md"

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

#line 97 "index.md"

	static void write_hex_byte(int byte) {
		
#line 125 "index.md"

	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		put("??");
	}

#line 99 "index.md"

	}

#line 52 "index.md"

	static void write_addr(
		const char *addr
	) {
		
#line 136 "index.md"

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

#line 56 "index.md"

	}

#line 152 "index.md"

	void dump_hex(const char *from,
		const char *to
	) {
		
#line 173 "index.md"

	put("\x1b[0E\x1b[2K");
	constexpr int bytes_per_row { 16 };
	if (from && from < to) {
		for (
			; from < to;
			from += bytes_per_row
		) {
			write_addr(from);
			put(": ");
			
#line 193 "index.md"
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
		
#line 226 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 204 "index.md"

	}
} 
#line 183 "index.md"

			put("| ");
			
#line 210 "index.md"
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
		
#line 226 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 220 "index.md"

	}
} 
#line 185 "index.md"

			putnl();
		}
	}

#line 156 "index.md"

	}

#line 241 "index.md"

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

#line 265 "index.md"

	void write_int(unsigned int v) {
		if (v >= 10) {
			write_int(v/10);
		}
		put((v % 10) + '0');
	}

#line 276 "index.md"

	struct Addr_State {
		char *ref;
		bool relative { false };
		bool negative { false };
		unsigned long value { 0 };
		unsigned digits { 0 };
		char *get(int &cmd) {
			if (cmd == EOF || cmd == '\n' || cmd == '.' || cmd == ' ') {
				relative = negative = false;
				value = reinterpret_cast<unsigned long>(ref);
				digits = 0;
			}

			bool done { false };
			for (;;) {
				switch (cmd) {
					case 0x7f:
						if (! digits) {
							if (relative) {
								put("\x1b[1D\x1b[K");
								relative = negative = false;
								break;
							}
							return nullptr;
						}
						--digits;
						put("\x1b[1D\x1b[K");
						value = value >> 4;
						break;
					case '+':
						if (digits || relative) { done = true; break; }
						put('+');
						relative = true;
						break;
					case '-':
						if (digits || relative) { done = true; break; }
						put('-');
						relative = true;
						negative = true;
						break;
					case '0': case '1': case '2': case '3': case '4':
					case '5': case '6': case '7': case '8': case '9':
					case 'a': case 'b': case 'c': case 'd': case 'e':
					case 'f': {
						if (! value) {
							if (digits) {
								put("\x1b[");
								write_int(digits);
								put("D\x1b[K");
								digits = 0;
							}
						}
						int v;
						if (cmd >= '0' && cmd <= '9') {
							v = cmd - '0';
						} else {
							v = cmd - 'a' + 10;
						}
						if (digits < 2 * sizeof(unsigned long)) {
							++digits;
							value = value * 16 + v;
							put(cmd);
						} else { put('\a'); }
						break;
					}
					default:
						done = true;
						break;
				}
				if (done) { break; }
				cmd = ::get();
			}
			char *res;
			auto r { reinterpret_cast<unsigned long>(ref) };
			if (! relative) {
				res = reinterpret_cast<char *>(value);
			} else if (negative) {
				if (value > r) {
					put('\a');
					res = nullptr;
				} else {
					res = reinterpret_cast<char *>(r - value);
				}
			} else {
				unsigned long mx { ~0ul };
				if (mx - value < r) {
					put('\a');
					res = reinterpret_cast<char *>(mx);
				} else {
					res = reinterpret_cast<char *>(r + value);
				}
			}
			if (digits + (relative ? 1 : 0)) {
				put("\x1b[");
				write_int(digits + (relative ? 1 : 0));
				put("D\x1b[K");
			}
			write_addr(res);
			value = reinterpret_cast<unsigned long>(res);
			relative = negative = false;
			digits = sizeof(unsigned long) * 2;

			return res;
		}
	};

#line 6 "index.md"

	int main() {
		
#line 62 "index.md"

	
#line 259 "index.md"
 
	Term_Handler term_handler;

#line 63 "index.md"

	char buffer[8 * 1024];
	char *addr { reinterpret_cast<char *>(
		buffer
	) };
	int cmd { 0 };
	while (cmd != EOF) {
		write_addr(addr);
		put("> ");
		cmd = get();
		if (cmd == EOF) { break; }
		while (cmd == 0x7f) {
			put('\a');
			cmd = get();
		}
		
#line 162 "index.md"

	if (cmd == '\n') {
		char *from { addr };
		addr += 8 * 16;
		dump_hex(from, addr);
		continue;
	}

#line 386 "index.md"

	if (cmd == 'h') {
		int state { 1 };
		put("hex ");
		cmd = get();
		Addr_State from;
		Addr_State to;
		from.ref = addr;
		for (;;) {
			if (state == 1) {
				if (cmd == 0x7f) {
					put("\x1b[0E\x1b[2K");
					break;
				}
				state = 2;
			}
			if (state == 2) {
				from.get(cmd);
				if (cmd == 0x7f) {
					state = 1;
				} else {
					to.ref = reinterpret_cast<char *>(from.value);
					state = 3;
				}
			}
			if (state == 3) {
				if (cmd == 0x7f) {
					put("\x1b[4D\x1b[K");
					cmd = get();
					state = 2;
				} else {
					put(" .. ");
					if (cmd == '.') {
						cmd = get();
					}
					state = 4;
				}
			}
			if (state == 4) {
				to.get(cmd);
				if (cmd == 0x7f) {
					state = 3;
				} else {
					putnl();
					dump_hex(reinterpret_cast<char*>(from.value), reinterpret_cast<char*>(to.value));
					addr = reinterpret_cast<char*>(to.value);
					if (cmd != '\n') {
						
#line 86 "index.md"

	put("\aunknown command ");
	put(isprint(cmd) ? (char) cmd : '?');
	put(" (");
	write_hex_byte(cmd & 0xff);
	put(')'); putnl();

#line 433 "index.md"
;
					}
					break;
				}
			}
		}
		continue;
	}

#line 78 "index.md"

		
#line 86 "index.md"

	put("\aunknown command ");
	put(isprint(cmd) ? (char) cmd : '?');
	put(" (");
	write_hex_byte(cmd & 0xff);
	put(')'); putnl();

#line 79 "index.md"

	}
	put("quit"); putnl();

#line 8 "index.md"

	}
