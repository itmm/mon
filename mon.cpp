
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

#line 233 "index.md"

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
	
#line 83 "index.md"

	
#line 92 "index.md"

	static void write_hex_nibble(
		int nibble
	) {
		
#line 102 "index.md"

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

#line 96 "index.md"

	}

#line 84 "index.md"

	static void write_hex_byte(int byte) {
		
#line 118 "index.md"

	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		put("??");
	}

#line 86 "index.md"

	}

#line 20 "index.md"

	static void write_addr(ulong addr) {
		
#line 129 "index.md"

	int shift { (sizeof(ulong) - 1) * 8 };
	for (; shift >= 0; shift -= 8) {
		write_hex_byte(
			(addr >> shift) & 0xff
		);
	}

#line 22 "index.md"

	}

#line 140 "index.md"

	constexpr int bytes_per_row { 16 };
	constexpr int default_rows { 8 };
	void dump_hex(ulong from, ulong to) {
		
#line 162 "index.md"

	put("\x1b[0E\x1b[2K");
	if (from && from < to) {
		for (
			; from < to;
			from += bytes_per_row
		) {
			
#line 177 "index.md"

	write_addr(from);
	put(": ");
	auto addr {
		reinterpret_cast<const char *>(
			from
		)
	};
	
#line 192 "index.md"
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
		
#line 225 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 203 "index.md"

	}
} 
#line 185 "index.md"

	put("| ");
	
#line 209 "index.md"
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
		
#line 225 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 219 "index.md"

	}
} 
#line 187 "index.md"


#line 169 "index.md"

			putnl();
		}
	}

#line 144 "index.md"

	}

#line 242 "index.md"

	#if UNIX_APP
		class Term_Handler {
			termios orig_;
		public:
			Term_Handler() {
				
#line 259 "index.md"

	tcgetattr(STDIN_FILENO, &orig_);
	termios raw { orig_ };
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(
		STDIN_FILENO, TCSAFLUSH, &raw
	);

#line 248 "index.md"

			}
			~Term_Handler() {
				
#line 270 "index.md"

	tcsetattr(STDIN_FILENO, TCSAFLUSH,
		&orig_
	);

#line 251 "index.md"

			}
		};
	#endif

#line 293 "index.md"

	void write_int(unsigned int v) {
		if (v >= 10) {
			write_int(v / 10);
		}
		put((v % 10) + '0');
	}

#line 304 "index.md"

	struct Addr_State {
		ulong ref;
		ulong dflt;
		bool relative { false };
		bool negative { false };
		ulong value { 0 };
		unsigned digits { 0 };
		ulong get(int &cmd);
	};
	
#line 389 "index.md"

	void delete_after_prev_chars(int n) {
		if (n > 0) {
			put("\x1b[");
			write_int(n);
			put("D\x1b[K");
		}
	}

#line 314 "index.md"

	ulong Addr_State::get(int &cmd) {
		
#line 322 "index.md"

	bool done { false };
	for (;;) {
		switch (cmd) {
		
#line 338 "index.md"

	case 0x7f:
		if (! digits) {
			if (! relative) {
				return 0;
			}
			
#line 356 "index.md"

	put("\x1b[1D\x1b[K");

#line 344 "index.md"

			relative = negative = false;
		} else {
			
#line 356 "index.md"

	put("\x1b[1D\x1b[K");

#line 347 "index.md"

			--digits;
			value = value >> 4;
		}
		break;

#line 362 "index.md"

	case '+': case '-':
		if (digits || relative) {
			done = true; break;
		}
		put(static_cast<char>(cmd));
		relative = true;
		negative = (cmd == '-');
		break;

#line 375 "index.md"

	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9': case 'a': case 'b':
	case 'c': case 'd': case 'e':
	case 'f': {
		
#line 401 "index.md"

	if (! value) {
		delete_after_prev_chars(digits);
		digits = 0;
	}

#line 411 "index.md"

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

#line 382 "index.md"

		break;
	}

#line 326 "index.md"

		default:
			done = true;
			break;
		}
		if (done) { break; }
		cmd = ::get();
	}

#line 427 "index.md"

	if (! relative) {
		
#line 439 "index.md"

	if (! digits) {
		value = dflt;
	}

#line 429 "index.md"

	} else if (negative) {
		
#line 447 "index.md"

	if (value > ref) {
		put('\a');
		value = 0;
	} else {
		value = ref - value;
	}

#line 431 "index.md"

	} else {
		
#line 458 "index.md"

	constexpr ulong mx { ~0ul };
	if (mx - value < ref) {
		put('\a');
		value = mx;
	} else {
		value = ref + value;
	}

#line 433 "index.md"

	}

#line 470 "index.md"

	delete_after_prev_chars(
		digits + (relative ? 1 : 0)
	);
	write_addr(value);
	relative = negative = false;
	digits = sizeof(ulong) * 2;
	return value;

#line 316 "index.md"

	}

#line 491 "index.md"

	enum class MC_State {
		before_enter_1st,
		entering_1st,
		after_entering_1st,
		entering_2nd
	};

#line 6 "index.md"

	int main() {
		
#line 28 "index.md"

	
#line 278 "index.md"
 
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

	const ulong start {
		reinterpret_cast<ulong>(
			write_addr
		)
	};
	ulong addr { start };

#line 40 "index.md"

	int cmd { 0 };
	#if ! UNIX_APP
		put("start with return");
		putnl();
		while (
			cmd != EOF && cmd != '\n'
		) {
			cmd = get();
		}
	#endif

#line 55 "index.md"

	while (cmd != EOF) {
		write_addr(addr);
		put("> ");
		cmd = get();
		if (cmd == EOF) { break; }
		while (cmd == 0x7f) {
			put('\a');
			cmd = get();
		}
		
#line 150 "index.md"

	if (cmd == '\n') {
		ulong from { addr };
		addr += default_rows *
			bytes_per_row;
		dump_hex(from, addr);
		continue;
	}

#line 482 "index.md"

	if (cmd == 'm') {
		
#line 502 "index.md"

	MC_State state { MC_State::before_enter_1st };
	put("memory ");
	cmd = get();
	Addr_State from;
	Addr_State to;
	from.ref = addr;
	from.dflt = addr;

#line 514 "index.md"

	bool done { false };
	while (! done) { switch (state) {
		case MC_State::before_enter_1st:
			
#line 540 "index.md"

	if (cmd == 0x7f) {
		
#line 534 "index.md"

	put("\x1b[0E\x1b[2K");

#line 542 "index.md"

		done = true;
	} else {
		state = MC_State::entering_1st;
	}

#line 518 "index.md"

			break;
		case MC_State::entering_1st:
			
#line 551 "index.md"

	from.get(cmd);
	if (cmd == 0x7f) {
		state =
			MC_State::before_enter_1st;
	} else {
		to.ref = from.value;
		to.dflt = from.value +
			bytes_per_row * default_rows;
		state =
			MC_State::after_entering_1st;
	}

#line 521 "index.md"

			break;
		case MC_State::after_entering_1st:
			
#line 567 "index.md"
 {
	constexpr char inter[] = " .. ";
	if (cmd == 0x7f) {
		delete_after_prev_chars(
			sizeof(inter) - 1
		);
		cmd = get();
		state = MC_State::entering_1st;
	} else {
		put(inter);
		if (cmd == '.') {
			cmd = get();
		}
		state = MC_State::entering_2nd;
	}
} 
#line 524 "index.md"

			break;
		case MC_State::entering_2nd:
			
#line 586 "index.md"

	to.get(cmd);
	if (cmd == 0x7f) {
		state =
			MC_State::after_entering_1st;
	} else {
		if (cmd == '\n') {
			
#line 604 "index.md"

	if (from.value <= to.value) {
		putnl();
		dump_hex(from.value, to.value);
		addr = to.value;
	} else {
		put('\a'); putnl();
	}

#line 593 "index.md"

			done = true;
		} else {
			put('\a');
			cmd = get();
		}
	}

#line 527 "index.md"

			break;
	} }

#line 484 "index.md"

		continue;
	}

#line 616 "index.md"

	if (cmd == 'x') {
		put("reset to start"); putnl();
		addr = start;
		continue;
	}

#line 65 "index.md"

		
#line 73 "index.md"

	put("\aunknown command ");
	put(isprint(cmd) ? (char) cmd : '?');
	put(" (");
	write_hex_byte(cmd & 0xff);
	put(')'); putnl();

#line 66 "index.md"

	}
	put("quit"); putnl();

#line 8 "index.md"

	}
