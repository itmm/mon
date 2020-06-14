
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

#line 238 "index.md"

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
	
#line 88 "index.md"

	
#line 97 "index.md"

	static void write_hex_nibble(
		int nibble
	) {
		
#line 107 "index.md"

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

#line 101 "index.md"

	}

#line 89 "index.md"

	static void write_hex_byte(int byte) {
		
#line 123 "index.md"

	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		put("??");
	}

#line 91 "index.md"

	}

#line 20 "index.md"

	static void write_addr(ulong addr) {
		
#line 134 "index.md"

	int shift { (sizeof(ulong) - 1) * 8 };
	for (; shift >= 0; shift -= 8) {
		write_hex_byte(
			(addr >> shift) & 0xff
		);
	}

#line 22 "index.md"

	}

#line 145 "index.md"

	constexpr int bytes_per_row { 16 };
	constexpr int default_rows { 8 };
	void dump_hex(ulong from, ulong to) {
		
#line 167 "index.md"

	put("\x1b[0E\x1b[2K");
	if (from && from < to) {
		for (
			; from < to;
			from += bytes_per_row
		) {
			
#line 182 "index.md"

	write_addr(from);
	put(": ");
	auto addr {
		reinterpret_cast<const char *>(
			from
		)
	};
	
#line 197 "index.md"
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
		
#line 230 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 208 "index.md"

	}
} 
#line 190 "index.md"

	put("| ");
	
#line 214 "index.md"
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
		
#line 230 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 224 "index.md"

	}
} 
#line 192 "index.md"


#line 174 "index.md"

			putnl();
		}
	}

#line 149 "index.md"

	}

#line 247 "index.md"

	#if UNIX_APP
		class Term_Handler {
			termios orig_;
		public:
			Term_Handler() {
				
#line 264 "index.md"

	tcgetattr(STDIN_FILENO, &orig_);
	termios raw { orig_ };
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(
		STDIN_FILENO, TCSAFLUSH, &raw
	);

#line 253 "index.md"

			}
			~Term_Handler() {
				
#line 275 "index.md"

	tcsetattr(STDIN_FILENO, TCSAFLUSH,
		&orig_
	);

#line 256 "index.md"

			}
		};
	#endif

#line 283 "index.md"

	#if ! UNIX_APP
		volatile int *prci {
			reinterpret_cast<
				volatile int *
			>(0x10008000) };
	#endif

#line 316 "index.md"

	void write_int(unsigned int v) {
		if (v >= 10) {
			write_int(v / 10);
		}
		put((v % 10) + '0');
	}

#line 327 "index.md"

	struct Addr_State {
		ulong ref;
		ulong dflt;
		bool relative { false };
		bool negative { false };
		ulong value { 0 };
		unsigned digits { 0 };
		ulong get(int &cmd);
	};
	
#line 412 "index.md"

	void delete_after_prev_chars(int n) {
		if (n > 0) {
			put("\x1b[");
			write_int(n);
			put("D\x1b[K");
		}
	}

#line 337 "index.md"

	ulong Addr_State::get(int &cmd) {
		
#line 345 "index.md"

	bool done { false };
	for (;;) {
		switch (cmd) {
		
#line 361 "index.md"

	case 0x7f:
		if (! digits) {
			if (! relative) {
				return 0;
			}
			
#line 379 "index.md"

	put("\x1b[D\x1b[K");

#line 367 "index.md"

			relative = negative = false;
		} else {
			
#line 379 "index.md"

	put("\x1b[D\x1b[K");

#line 370 "index.md"

			--digits;
			value = value >> 4;
		}
		break;

#line 385 "index.md"

	case '+': case '-':
		if (digits || relative) {
			done = true; break;
		}
		put(static_cast<char>(cmd));
		relative = true;
		negative = (cmd == '-');
		break;

#line 398 "index.md"

	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9': case 'a': case 'b':
	case 'c': case 'd': case 'e':
	case 'f': {
		
#line 424 "index.md"

	if (! value) {
		delete_after_prev_chars(digits);
		digits = 0;
	}

#line 434 "index.md"

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

#line 405 "index.md"

		break;
	}

#line 349 "index.md"

		default:
			done = true;
			break;
		}
		if (done) { break; }
		cmd = ::get();
	}

#line 450 "index.md"

	if (! relative) {
		
#line 462 "index.md"

	if (! digits) {
		value = dflt;
	}

#line 452 "index.md"

	} else if (negative) {
		
#line 470 "index.md"

	if (value > ref) {
		put('\a');
		value = 0;
	} else {
		value = ref - value;
	}

#line 454 "index.md"

	} else {
		
#line 481 "index.md"

	constexpr ulong mx { ~0ul };
	if (mx - value < ref) {
		put('\a');
		value = mx;
	} else {
		value = ref + value;
	}

#line 456 "index.md"

	}

#line 493 "index.md"

	delete_after_prev_chars(
		digits + (relative ? 1 : 0)
	);
	write_addr(value);
	relative = negative = false;
	digits = sizeof(ulong) * 2;
	return value;

#line 339 "index.md"

	}

#line 514 "index.md"

	enum class MC_State {
		before_enter_1st,
		entering_1st,
		after_entering_1st,
		entering_2nd
	};

#line 658 "index.md"

	void write_hex_int(int value, int bytes) {
		if (bytes < 0) {
			if (value < 256) { write_hex_int(value, 1); }
			else if (value < 0x10000) { write_hex_int(value, 2); }
			else { write_hex_int(value, 4); }
			return;
		}
		for (; bytes; --bytes) {
			write_hex_byte((value >> (8 * (bytes - 1))) & 0xff);
		}
	}

#line 674 "index.md"

	void write_reg(int reg) {
		switch (reg) {
			case 0: put("zero"); break;
			case 1: put("ra"); break;
			case 2: put("sp"); break;
			case 3: put("gp"); break;
			case 4: put("tp"); break;
			case 5: case 6: case 7:
				put('t'); put('0' + reg - 5); break;
			case 8: case 9:
				put('s'); put('0' + reg - 8); break;
			case 10: case 11: case 12:
			case 13: case 14: case 15:
			case 16: case 17:
				put('a'); put('0' + reg - 10); break;
			case 18: case 19: case 20:
			case 21: case 22: case 23:
			case 24: case 25: case 26:
			case 27:
				put('s'); write_int(reg - 18 + 2); break;
			case 28: case 29: case 30:
			case 31:
				put('t'); put('0' + reg - 31 + 3); break;
			default:
				put("?? # "); write_int(reg);
		}
	}

#line 6 "index.md"

	int main() {
		
#line 28 "index.md"

	
#line 294 "index.md"
 
	#if UNIX_APP
		Term_Handler term_handler;
	#else
		constexpr int hfrosccfg { 0x00 };
		constexpr int hfxosccfg { 0x01 };
		while (prci[hfxosccfg] >= 0) { }
		prci[hfrosccfg] &= ~0x40000000;

		constexpr int tx_control { 0x02 };
		uart[tx_control] |= 1;
		constexpr int rx_control { 0x03 };
		uart[rx_control] |= 1;
		constexpr int div { 0x18/4 };
		constexpr int rx_data { 0x01 };
		uart[div] = (uart[div] & ~0xffff) | 139;
		while (uart[rx_data] >= 0) {}
	#endif

#line 29 "index.md"

	#if UNIX_APP
		char buffer[] =
			"\x41\x11\x06\xc6\x22\xc4"
			"\x26\xc2\x4a\xc0\x2a\x84"
			"\xe1\x44\x7d\x59"
			"\x33\x55\x94\x00"
			"\x13\x75\xf5\x0f"
			"\x97\x00\x00\x00"
			"\xe7\x80\xa0\x01"
			"\xe1\x14\xe3\x47\x99\xfe"
			"\x02\x49\x92\x44\x22\x44"
			"\xb2\x40\x41\x01\x82\x80";
	const ulong start {
		reinterpret_cast<ulong>(
			buffer
		)
	};
	#else
	const ulong start {
		reinterpret_cast<ulong>(
			write_addr
		)
	};
	#endif
	ulong addr { start };

#line 59 "index.md"

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
		
#line 155 "index.md"

	if (cmd == '\n') {
		ulong from { addr };
		addr += default_rows *
			bytes_per_row;
		dump_hex(from, addr);
		continue;
	}

#line 505 "index.md"

	if (cmd == 'm') {
		
#line 525 "index.md"

	MC_State state { MC_State::before_enter_1st };
	put("memory ");
	cmd = get();
	Addr_State from;
	Addr_State to;
	from.ref = addr;
	from.dflt = addr;

#line 537 "index.md"

	bool done { false };
	while (! done) { switch (state) {
		case MC_State::before_enter_1st:
			
#line 563 "index.md"

	if (cmd == 0x7f) {
		
#line 557 "index.md"

	put("\x1b[G\x1b[K");

#line 565 "index.md"

		done = true;
	} else {
		state = MC_State::entering_1st;
	}

#line 541 "index.md"

			break;
		case MC_State::entering_1st:
			
#line 574 "index.md"

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

#line 544 "index.md"

			break;
		case MC_State::after_entering_1st:
			
#line 590 "index.md"
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
#line 547 "index.md"

			break;
		case MC_State::entering_2nd:
			
#line 609 "index.md"

	to.get(cmd);
	if (cmd == 0x7f) {
		state =
			MC_State::after_entering_1st;
	} else {
		if (cmd == '\n') {
			
#line 627 "index.md"

	if (from.value <= to.value) {
		putnl();
		dump_hex(from.value, to.value);
		addr = to.value;
	} else {
		put('\a'); putnl();
	}

#line 616 "index.md"

			done = true;
		} else {
			put('\a');
			cmd = get();
		}
	}

#line 550 "index.md"

			break;
	} }

#line 507 "index.md"

		continue;
	}

#line 639 "index.md"

	if (cmd == 'x') {
		put("reset to start"); putnl();
		addr = start;
		continue;
	}

#line 649 "index.md"

	if (cmd == 'd') {
		
#line 706 "index.md"

	
#line 557 "index.md"

	put("\x1b[G\x1b[K");

#line 707 "index.md"

	write_addr(addr);
	put("  ");
	const auto bytes { reinterpret_cast<
		unsigned char *
	>(addr) };
	unsigned cmd { 0 };
	cmd = bytes[0];
	if ((cmd & 0x3) != 0x3) {
		cmd |= bytes[1] << 8;
			
		write_hex_byte(bytes[0]);
		put(' ');
		write_hex_byte(bytes[1]);
		put("        ");
		if (cmd == 0x0000) {
			put("db.s $0000 # illegal");
		} else if ((cmd & 0xe003) == 0x0001 && (cmd & 0x0f80) != 0) {
			auto reg { (cmd & 0x0f80) >> 7 };
			auto immed { (cmd & 0x007c) >> 2 };
			write_reg(reg);
			put(" <- ");
			write_reg(reg);
			if (cmd & 0x1000) {
				immed = (~immed + 1) & 0x1f;
				put(" - $");
			} else {
				put(" + $");
			}
			write_hex_int(immed, -1);
		} else if ((cmd & 0xe003) == 0x4001 && (cmd & 0x0f80) != 0) {
			auto reg { (cmd & 0x0f80) >> 7 };
			auto immed { (cmd & 0x007c) >> 2 };
			write_reg(reg);
			put(" <- ");
			if (cmd & 0x1000) {
				immed = (~immed + 1) & 0x1f;
				put("-$");
			} else {
				put('$');
			}
			write_hex_int(immed, -1);
		} else if ((cmd & 0xe003) == 0xc002) {
			auto reg { (cmd & 0x7c) >> 2 };
			auto offset { ((cmd & 0x1e00) >> (9 - 2)) | ((cmd & 0x0180) >> (7 - 6)) };
			put("[sp + $");
			write_hex_int(offset, -1);
			put("] <- ");
			write_reg(reg);
		} else if ((cmd & 0xe003) == 0x4002) {
			auto reg { (cmd & 0x0f80) >> 7 };
			auto offset { ((cmd & 0x0070) >> (4 - 2)) | ((cmd & 0x000c) << (6 - 2)) | ((cmd & 0x1000) >> (12 - 5)) };
			write_reg(reg);
			put(" <- [sp + $");
			write_hex_int(offset, -1);
			put(']');
		} else if ((cmd & 0xf003) == 0x8002 && (cmd & 0x007c) != 0) {
			write_reg((cmd & 0x0f80) >> 7);
			put(" <- ");
			write_reg((cmd & 0x007c) >> 2);
			
		} else if ((cmd & 0xf003) == 0x9002 && (cmd & 0x007c) != 0) {
			write_reg((cmd & 0x0f80) >> 7);
			put(" <- ");
			write_reg((cmd & 0x0f80) >> 7);
			put(" + ");
			write_reg((cmd & 0x007c) >> 2);
			
		} else if (cmd == 0x8082) {
			put("ret");
		} else {
			put("db.s $");
			write_hex_int(cmd, 2);
		}
		putnl();
		addr += 2;
	} else if ((cmd & 0xc) != 0xc) {
		cmd |= bytes[1] << 8;
		cmd |= bytes[2] << 16;
		cmd |= bytes[3] << 24;
		write_hex_byte(bytes[0]);
		put(' ');
		write_hex_byte(bytes[1]);
		put(' ');
		write_hex_byte(bytes[2]);
		put(' ');
		write_hex_byte(bytes[3]);
		put("  ");
		if ((cmd & 0xfe00707f) == 0x00005033) {
			write_reg((cmd & 0x00000f80) >> 7);
			put(" <- ");
			write_reg((cmd & 0x000f8000) >> 15);
			put(" >> ");
			write_reg((cmd & 0x01f00000) >> 20);
		} else {
			put("db.w $");
			write_hex_int(cmd, 4);
		}
		putnl();
		addr += 4;
	} else {
		write_hex_byte(bytes[0]);
		put(' ');
		write_hex_byte(bytes[1]);
		put("        ");
		put("db.b $");
		write_hex_byte(bytes[0]);
		put(" $");
		write_hex_byte(bytes[1]);
		putnl();
		addr += 2;
	}

#line 651 "index.md"

		continue;
	}

#line 70 "index.md"

		
#line 78 "index.md"

	put("\aunknown command ");
	put(isprint(cmd) ? (char) cmd : '?');
	put(" (");
	write_hex_byte(cmd & 0xff);
	put(')'); putnl();

#line 71 "index.md"

	}
	put("quit"); putnl();

#line 8 "index.md"

	}
