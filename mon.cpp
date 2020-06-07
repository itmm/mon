
#line 4 "index.md"

	
#line 4 "io.md"

	#if UNIX_APP
		#include <iostream>
	#else
		
#line 14 "io.md"

	constexpr int EOF { -1 };
	inline bool isprint(char ch) {
		return ch >= ' ' && ch <= '~';
	}

#line 8 "io.md"

	#endif

#line 202 "index.md"

	#if UNIX_APP
		#include <termios.h>
		#include <unistd.h>
	#endif

#line 5 "index.md"

	
#line 23 "io.md"

	
#line 32 "io.md"

	#if ! UNIX_APP
		volatile int *uart {
			reinterpret_cast<
				volatile int *
			>(0x10013000) };
	#endif

#line 24 "io.md"

	void put(char ch) {
		
#line 43 "io.md"

	#if UNIX_APP
		std::cout.put(ch);
	#else
		constexpr int tx_data { 0x00 };
		while (uart[tx_data] < 0) {}
		uart[tx_data] = ch;
	#endif

#line 26 "io.md"

	}

#line 55 "io.md"

	void put(const char *begin) {
		if (begin) {
			while (*begin) {
				put(*begin++);
			}
		}
	}

#line 67 "io.md"

	void putnl() {
		#if UNIX_APP
			put('\n');
		#else
			put("\r\n");
		#endif
	}

#line 79 "io.md"

	int get() {
		
#line 87 "io.md"

	#if UNIX_APP
		int res { std::cin.get() };
		if (res == 0x04) { res = EOF; }
	#else
		constexpr int rx_data { 0x01 };
		int res { EOF };
		
#line 103 "io.md"

	while (res < 0) {
		res = uart[rx_data];
	}
	res = res & 0xff;

#line 94 "io.md"

		res = res & 0xff;
	#endif
	if (res == '\r') { res = '\n'; }
	return res;

#line 81 "io.md"

	}

#line 18 "index.md"

	using ulong = unsigned long;
	
#line 68 "index.md"

	
#line 77 "index.md"

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

#line 69 "index.md"

	static void write_hex_byte(int byte) {
		
#line 97 "index.md"

	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		put("??");
	}

#line 71 "index.md"

	}

#line 20 "index.md"

	static void write_addr(ulong addr) {
		
#line 108 "index.md"

	int shift { (sizeof(ulong) - 1) * 8 };
	for (; shift >= 0; shift -= 8) {
		write_hex_byte(
			(addr >> shift) & 0xff
		);
	}

#line 22 "index.md"

	}

#line 119 "index.md"

	constexpr int bytes_per_row { 16 };
	constexpr int default_rows { 8 };
	void dump_hex(ulong from, ulong to) {
		
#line 141 "index.md"

	put("\x1b[0E\x1b[2K");
	if (from && from < to) {
		for (
			; from < to;
			from += bytes_per_row
		) {
			write_addr(from);
			put(": ");
			auto addr { reinterpret_cast<const char *>(from) };
			
#line 161 "index.md"
 {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		if (from + row < to) {
			write_hex_byte(
				addr[row] & 0xff
			);
		} else {
			put("  ");
		}
		put(' ');
		
#line 194 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 172 "index.md"

	}
} 
#line 151 "index.md"

			put("| ");
			
#line 178 "index.md"
 {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		if (from + row >= to) {
			put(' ');
		} else if (isprint(addr[row])) {
			put(addr[row]);
		} else {
			put('.');
		}
		
#line 194 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 188 "index.md"

	}
} 
#line 153 "index.md"

			putnl();
		}
	}

#line 123 "index.md"

	}

#line 211 "index.md"

	#if UNIX_APP
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
	#endif

#line 246 "index.md"

	void write_int(unsigned int v) {
		if (v >= 10) {
			write_int(v/10);
		}
		put((v % 10) + '0');
	}

#line 257 "index.md"

	struct Addr_State {
		ulong ref;
		ulong dflt;
		bool relative { false };
		bool negative { false };
		ulong value { 0 };
		unsigned digits { 0 };
		ulong get(int &cmd) {
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
							return 0;
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
						if (digits < 2 * sizeof(ulong)) {
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
			ulong res;
			if (! relative) {
				if (! digits) {
					value = dflt;
				}
				res = value;
			} else if (negative) {
				if (value > ref) {
					put('\a');
					res = 0;
				} else {
					res = ref - value;
				}
			} else {
				ulong mx { ~0ul };
				if (mx - value < ref) {
					put('\a');
					res = mx;
				} else {
					res = ref + value;
				}
			}
			if (digits + (relative ? 1 : 0)) {
				put("\x1b[");
				write_int(digits + (relative ? 1 : 0));
				put("D\x1b[K");
			}
			write_addr(res);
			value = res;
			relative = negative = false;
			digits = sizeof(unsigned long) * 2;

			return res;
		}
	};

#line 6 "index.md"

	int main() {
		
#line 28 "index.md"

	
#line 231 "index.md"
 
	#if UNIX_APP
		Term_Handler term_handler;
	#else
		constexpr int tx_control { 0x02 };
		uart[tx_control] |= 1;
		constexpr int rx_control { 0x03 };
		uart[rx_control] |= 1;
		constexpr int rx_data { 0x01 };
		while (uart[rx_data] >= 0) {}
	#endif

#line 29 "index.md"

	ulong start { reinterpret_cast<ulong>(
		write_addr
	) };
	ulong addr { start };
	int cmd { 0 };
	#if ! UNIX_APP
		put("start with return"); putnl();
		while (cmd != EOF && cmd != '\n') {
			cmd = get();
		}
	#endif
	while (cmd != EOF) {
		write_addr(addr);
		put("> ");
		cmd = get();
		if (cmd == EOF) { break; }
		while (cmd == 0x7f) {
			put('\a');
			cmd = get();
		}
		
#line 129 "index.md"

	if (cmd == '\n') {
		ulong from { addr };
		addr += default_rows *
			bytes_per_row;
		dump_hex(from, addr);
		continue;
	}

#line 364 "index.md"

	if (cmd == 'm') {
		int state { 1 };
		put("memory ");
		cmd = get();
		Addr_State from;
		Addr_State to;
		from.ref = addr;
		from.dflt = addr;
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
					to.ref = from.value;
					to.dflt = from.value + bytes_per_row * default_rows;
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
					if (cmd == '\n') {
						if (from.value <= to.value) {
							putnl();
							dump_hex(from.value, to.value);
							addr = to.value;
						} else {
							put('\a'); putnl();
						}
						break;
					} else {
						put('\a');
						cmd = get();
					}
				}
			}
		}
		continue;
	}

#line 431 "index.md"

	if (cmd == 'x') {
		put("reset to start"); putnl();
		addr = start;
		continue;
	}

#line 50 "index.md"

		
#line 58 "index.md"

	put("\aunknown command ");
	put(isprint(cmd) ? (char) cmd : '?');
	put(" (");
	write_hex_byte(cmd & 0xff);
	put(')'); putnl();

#line 51 "index.md"

	}
	put("quit"); putnl();

#line 8 "index.md"

	}
