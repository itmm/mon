
#line 6 "index.md"

	
#line 6 "io.md"

	#if UNIX_APP
		#include <iostream>
	#else
		
#line 18 "io.md"

	constexpr int EOF { -1 };
	inline bool isprint(char ch) {
		return ch >= ' ' && ch <= '~';
	}

#line 10 "io.md"

	#endif

#line 342 "index.md"

	#if UNIX_APP
		#include <termios.h>
		#include <unistd.h>
	#endif

#line 7 "index.md"

	
#line 30 "io.md"

	
#line 40 "io.md"

	#if ! UNIX_APP
		volatile int *uart {
			reinterpret_cast<
				volatile int *
			>(0x10013000) };
	#endif

#line 31 "io.md"

	void put(char ch) {
		
#line 52 "io.md"

	#if UNIX_APP
		std::cout.put(ch);
	#else
		constexpr int tx_data { 0x00 };
		while (uart[tx_data] < 0) {}
		uart[tx_data] = ch;
	#endif

#line 33 "io.md"

	}

#line 67 "io.md"

	void put(const char *begin) {
		if (begin) {
			while (*begin) {
				put(*begin++);
			}
		}
	}

#line 81 "io.md"

	void putnl() {
		#if UNIX_APP
			put('\n');
		#else
			put("\r\n");
		#endif
	}

#line 94 "io.md"

	int get() {
		
#line 103 "io.md"

	#if UNIX_APP
		int res { std::cin.get() };
		if (res == 0x04) { res = EOF; }
	#else
		constexpr int rx_data { 0x01 };
		int res { EOF };
		
#line 123 "io.md"

	while (res < 0) {
		res = uart[rx_data];
	}
	res = res & 0xff;

#line 110 "io.md"

		res = res & 0xff;
	#endif
	if (res == '\r') { res = '\n'; }
	return res;

#line 96 "io.md"

	}

#line 22 "index.md"

	using ulong = unsigned long;
	using uchr = unsigned char;

#line 79 "index.md"

	
#line 137 "index.md"

	
#line 159 "index.md"

	static void write_hex_nibble(
		int nibble
	) {
		
#line 183 "index.md"

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

#line 163 "index.md"

	}

#line 138 "index.md"

	static void write_hex_byte(int byte) {
		
#line 170 "index.md"

	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		put("??");
	}

#line 140 "index.md"

	}

#line 80 "index.md"

	static void write_addr(ulong addr) {
		
#line 147 "index.md"

	int shift { (sizeof(ulong) - 1) * 8 };
	for (; shift >= 0; shift -= 8) {
		write_hex_byte(
			(addr >> shift) & 0xff
		);
	}

#line 82 "index.md"

	}

#line 218 "index.md"

	constexpr int bytes_per_row { 16 };
	constexpr int default_rows { 8 };
	void dump_hex(ulong from, ulong to) {
		
#line 246 "index.md"

	
#line 265 "index.md"

	put("\x1b[G\x1b[K");

#line 247 "index.md"

	if (from && from < to) {
		for (
			; from < to;
			from += bytes_per_row
		) {
			
#line 273 "index.md"

	write_addr(from);
	put(": ");
	auto addr {
		reinterpret_cast<const char *>(
			from
		)
	};
	
#line 291 "index.md"
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
		
#line 330 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 302 "index.md"

	}
} 
#line 281 "index.md"

	put("| ");
	
#line 311 "index.md"
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
		
#line 330 "index.md"

	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }

#line 321 "index.md"

	}
} 
#line 283 "index.md"


#line 253 "index.md"

			putnl();
		}
	}

#line 222 "index.md"

	}

#line 353 "index.md"

	#if UNIX_APP
		class Term_Handler {
			termios orig_;
		public:
			Term_Handler() {
				
#line 372 "index.md"

	tcgetattr(STDIN_FILENO, &orig_);
	termios raw { orig_ };
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(
		STDIN_FILENO, TCSAFLUSH, &raw
	);

#line 359 "index.md"

			}
			~Term_Handler() {
				
#line 385 "index.md"

	tcsetattr(STDIN_FILENO, TCSAFLUSH,
		&orig_
	);

#line 362 "index.md"

			}
		};
	#endif

#line 405 "index.md"

	#if ! UNIX_APP
		volatile int *prci {
			reinterpret_cast<
				volatile int *
			>(0x10008000) };
	#endif

#line 458 "index.md"

	struct Addr_State {
		
#line 468 "index.md"

	ulong ref;

#line 475 "index.md"

	ulong dflt;

#line 482 "index.md"

	bool relative { false };

#line 489 "index.md"

	bool negative { false };

#line 496 "index.md"

	ulong value { 0 };

#line 503 "index.md"

	unsigned digits { 0 };

#line 460 "index.md"

		ulong get(int &cmd);
	};

#line 510 "index.md"

	
#line 595 "index.md"

	void write_int(unsigned int v) {
		if (v >= 10) {
			write_int(v / 10);
		}
		put((v % 10) + '0');
	}

#line 608 "index.md"

	void delete_after_prev_chars(int n) {
		if (n > 0) {
			put("\x1b[");
			write_int(n);
			put("D\x1b[K");
		}
	}

#line 511 "index.md"

	ulong Addr_State::get(int &cmd) {
		
#line 520 "index.md"

	bool done { false };
	for (;;) {
		switch (cmd) {
		
#line 538 "index.md"

	case 0x7f:
		if (! digits) {
			if (! relative) {
				return 0;
			}
			
#line 558 "index.md"

	put("\x1b[D\x1b[K");

#line 544 "index.md"

			relative = negative = false;
		} else {
			
#line 558 "index.md"

	put("\x1b[D\x1b[K");

#line 547 "index.md"

			--digits; value = value >> 4;
		}
		break;

#line 565 "index.md"

	case '+': case '-':
		if (digits || relative) {
			done = true; break;
		}
		put(static_cast<char>(cmd));
		relative = true;
		negative = (cmd == '-');
		break;

#line 580 "index.md"

	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9': case 'a': case 'b':
	case 'c': case 'd': case 'e':
	case 'f': {
		
#line 622 "index.md"

	if (! value) {
		delete_after_prev_chars(digits);
		digits = 0;
	}

#line 634 "index.md"

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

#line 587 "index.md"

		break;
	}

#line 524 "index.md"

		default:
			done = true;
			break;
		}
		if (done) { break; }
		cmd = ::get();
	}

#line 653 "index.md"

	if (! relative) {
		
#line 666 "index.md"

	if (! digits) {
		value = dflt;
	}

#line 655 "index.md"

	} else if (negative) {
		
#line 675 "index.md"

	if (value > ref) {
		put('\a');
		value = 0;
	} else {
		value = ref - value;
	}

#line 657 "index.md"

	} else {
		
#line 688 "index.md"

	constexpr ulong mx { ~0ul };
	if (mx - value < ref) {
		put('\a');
		value = mx;
	} else {
		value = ref + value;
	}

#line 659 "index.md"

	}

#line 702 "index.md"

	delete_after_prev_chars(
		digits + (relative ? 1 : 0)
	);
	write_addr(value);
	relative = negative = false;
	digits = sizeof(ulong) * 2;
	return value;

#line 513 "index.md"

	}

#line 727 "index.md"

	enum class MC_State {
		
#line 736 "index.md"

	before_enter_1st

#line 743 "index.md"
,
	entering_1st

#line 750 "index.md"
,
	after_entering_1st

#line 757 "index.md"
,
	entering_2nd

#line 729 "index.md"

	};

#line 5 "disassembler.md"

	void write_hex_int(
		int value, int bytes
	) {
		
#line 19 "disassembler.md"

	if (bytes < 0) {
		
#line 41 "disassembler.md"

	if (value < 256) {
		write_hex_int(value, 1);
	} else if (value < 0x10000) {
		write_hex_int(value, 2);
	} else {
		write_hex_int(value, 4);
	}

#line 21 "disassembler.md"

		return;
	}

#line 29 "disassembler.md"

	for (; bytes; --bytes) {
		write_hex_byte(
			(value >> (8 * (bytes - 1))) &
			0xff
		);
	}

#line 9 "disassembler.md"

	}

#line 54 "disassembler.md"

	void write_reg(int reg) {
		switch (reg) {
			
#line 68 "disassembler.md"

	case 0: put("%zero"); break;
	case 1: put("%ra"); break;
	case 2: put("%sp"); break;
	case 3: put("%gp"); break;
	case 4: put("%tp"); break;

#line 79 "disassembler.md"

	case 5: case 6: case 7:
		put("%t");
		put('0' + reg - 5);
		break;

#line 89 "disassembler.md"

	case 8: case 9:
		put("%s");
		put('0' + reg - 8);
		break;

#line 99 "disassembler.md"

	case 10: case 11: case 12:
	case 13: case 14: case 15:
	case 16: case 17:
		put("%a");
		put('0' + reg - 10);
		break;

#line 111 "disassembler.md"

	case 18: case 19: case 20:
	case 21: case 22: case 23:
	case 24: case 25: case 26:
	case 27:
		put("%s");
		write_int(reg - 18 + 2);
		break;

#line 124 "disassembler.md"

	case 28: case 29: case 30:
	case 31:
		put("%t");
		put('0' + reg - 31 + 3);
		break;

#line 57 "disassembler.md"

			default:
				put("?? # ");
				write_int(reg);
		}
	}

#line 173 "disassembler.md"

	void write_hex_bytes(
		const uchr *bytes,
		int count
	) {
		for (; count; --count, ++bytes) {
			write_hex_byte(*bytes);
			put(' ');
		}
	}

#line 8 "index.md"

	int main() {
		
#line 33 "index.md"

	
#line 394 "index.md"
 
	#if UNIX_APP
		Term_Handler term_handler;
	#else
		
#line 417 "index.md"

	constexpr int hfrosccfg { 0x00/4 };
	constexpr int hfxosccfg { 0x04/4 };
	while (prci[hfxosccfg] >= 0) { }
	prci[hfrosccfg] &= ~0x40000000;

#line 428 "index.md"

	constexpr int tx_control { 0x08/4 };
	uart[tx_control] |= 1;
	constexpr int rx_control { 0x0c/4 };
	uart[rx_control] |= 1;

#line 438 "index.md"

	constexpr int div { 0x18/4 };
	uart[div] =
		(uart[div] & ~0xffff) | 139;

#line 447 "index.md"

	constexpr int rx_data { 0x01 };
	while (uart[rx_data] >= 0) {}

#line 398 "index.md"

	#endif

#line 34 "index.md"

	#if UNIX_APP
		
#line 48 "index.md"

	char buffer[] = ""
		
#line 63 "index.md"

	"\x41\x11\x06\xc6\x22\xc4"
	"\x26\xc2\x4a\xc0\x2a\x84"
	"\xe1\x44\x7d\x59"
	"\x33\x55\x94\x00"
	"\x13\x75\xf5\x0f"
	"\x97\x00\x00\x00"
	"\xe7\x80\xa0\x01"
	"\xe1\x14\xe3\x47\x99\xfe"
	"\x02\x49\x92\x44\x22\x44"
	"\xb2\x40\x41\x01\x82\x80"

#line 50 "index.md"
;
	const ulong start {
		reinterpret_cast<ulong>(
			buffer
		)
	};

#line 36 "index.md"

	#else
		
#line 89 "index.md"

	const ulong start {
		reinterpret_cast<ulong>(
			write_addr
		)
	};

#line 38 "index.md"

	#endif
	ulong addr { start };

#line 103 "index.md"

	int cmd { 0 };
	while (cmd != EOF) {
		write_addr(addr);
		put("> ");
		cmd = get();
		if (cmd == EOF) { break; }
		
#line 203 "index.md"

	while (cmd == 0x7f) {
		put('\a'); cmd = get();
	}

#line 230 "index.md"

	if (cmd == '\n') {
		ulong from { addr };
		addr += default_rows *
			bytes_per_row;
		dump_hex(from, addr);
		continue;
	}

#line 717 "index.md"

	if (cmd == 'm') {
		
#line 764 "index.md"

	MC_State state {
		MC_State::before_enter_1st
	};
	put("memory ");
	cmd = get();

#line 777 "index.md"

	Addr_State from;
	Addr_State to;
	from.ref = addr;
	from.dflt = addr;

#line 787 "index.md"

	bool done { false };
	while (! done) { switch (state) {
		
#line 797 "index.md"

	case MC_State::before_enter_1st:
		
#line 815 "index.md"

	if (cmd == 0x7f) {
		
#line 265 "index.md"

	put("\x1b[G\x1b[K");

#line 817 "index.md"

		done = true;
	} else {
		state = MC_State::entering_1st;
	}

#line 799 "index.md"

		break;
	case MC_State::entering_1st:
		
#line 828 "index.md"

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

#line 802 "index.md"

		break;
	case MC_State::after_entering_1st:
		
#line 850 "index.md"
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
		if (cmd == '.') { cmd = get(); }
		state = MC_State::entering_2nd;
	}
} 
#line 805 "index.md"

		break;
	case MC_State::entering_2nd:
		
#line 870 "index.md"

	to.get(cmd);
	if (cmd == 0x7f) {
		state =
			MC_State::after_entering_1st;
	} else {
		if (cmd == '\n') {
			
#line 890 "index.md"

	if (from.value <= to.value) {
		putnl();
		dump_hex(from.value, to.value);
		addr = to.value;
	} else {
		put('\a'); putnl();
	}

#line 877 "index.md"

			done = true;
		} else {
			put('\a'); cmd = get();
		}
	}

#line 808 "index.md"

		break;

#line 790 "index.md"

	} }

#line 719 "index.md"

		continue;
	}

#line 906 "index.md"

	if (cmd == 'x') {
		put("reset to start"); putnl();
		addr = start;
		continue;
	}

#line 917 "index.md"

	if (cmd == 'd') {
		
#line 135 "disassembler.md"

	
#line 265 "index.md"

	put("\x1b[G\x1b[K");

#line 136 "disassembler.md"

	write_addr(addr);
	put("  ");

#line 144 "disassembler.md"

	const auto bytes { reinterpret_cast<
		uchr *
	>(addr) };
	unsigned cmd { 0 };
	cmd = bytes[0];

#line 156 "disassembler.md"

	if ((cmd & 0x3) != 0x3) {
		
#line 188 "disassembler.md"

	write_hex_bytes(bytes, 2);
	put("       ");
	cmd |= bytes[1] << 8;

#line 198 "disassembler.md"

	if (cmd == 0x0000) {
		put("db.s $0000 // illegal");
	}
	
#line 212 "disassembler.md"

	else if ((cmd & 0xe003) == 0x0001 &&
		(cmd & 0x0f80)
	) {
		
#line 223 "disassembler.md"

	auto reg { (cmd & 0x0f80) >> 7 };
	auto immed { (cmd & 0x007c) >> 2 };
	write_reg(reg);
	put(" <- ");
	write_reg(reg);
	if (cmd & 0x1000) {
		immed = (-immed) & 0x1f;
		put(" - $");
	} else {
		put(" + $");
	}
	write_hex_int(immed, -1);

#line 216 "disassembler.md"

	}

#line 242 "disassembler.md"

	else if ((cmd & 0xe003) == 0x4001 &&
		(cmd & 0x0f80)
	) {
		
#line 253 "disassembler.md"

	auto reg { (cmd & 0x0f80) >> 7 };
	auto immed { (cmd & 0x007c) >> 2 };
	write_reg(reg);
	put(" <- ");
	if (cmd & 0x1000) {
		immed = (-immed) & 0x1f;
		put("-$");
	} else {
		put('$');
	}
	write_hex_int(immed, -1);

#line 246 "disassembler.md"

	}

#line 271 "disassembler.md"

	else if ((cmd & 0xe003) == 0xc002) {
		auto reg { (cmd & 0x7c) >> 2 };
		auto offset {
			((cmd & 0x1e00) >> (9 - 2)) |
			((cmd & 0x0180) >> (7 - 6))
		};
		put("[%sp + $");
		write_hex_int(offset, -1);
		put("] <- ");
		write_reg(reg);
	}

#line 288 "disassembler.md"

	else if ((cmd & 0xe003) == 0x4002) {
		auto reg { (cmd & 0x0f80) >> 7 };
		auto offset {
			((cmd & 0x0070) >> (4 - 2)) |
			((cmd & 0x000c) << (6 - 2)) |
			((cmd & 0x1000) >> (12 - 5))
		};
		write_reg(reg);
		put(" <- [%sp + $");
		write_hex_int(offset, -1);
		put(']');
	}

#line 306 "disassembler.md"

	else if ((cmd & 0xf003) == 0x8002 &&
		(cmd & 0x007c)
	) {
		write_reg((cmd & 0x0f80) >> 7);
		put(" <- ");
		write_reg((cmd & 0x007c) >> 2);
	}

#line 319 "disassembler.md"

	else if ((cmd & 0xf003) == 0x9002 &&
		(cmd & 0x007c)
	) {
		write_reg((cmd & 0x0f80) >> 7);
		put(" <- ");
		write_reg((cmd & 0x0f80) >> 7);
		put(" + ");
		write_reg((cmd & 0x007c) >> 2);
	}

#line 334 "disassembler.md"

	else if (cmd == 0x8082) {
		put("%pc <- %ra // return");
	}

#line 202 "disassembler.md"

	else {
		put("db.s $");
		write_hex_int(cmd, 2);
	}

#line 158 "disassembler.md"

		addr += 2;
	} else if ((cmd & 0xc) != 0xc) {
		
#line 343 "disassembler.md"

	write_hex_bytes(bytes, 4);
	put(' ');
	cmd |= bytes[1] << 8;
	cmd |= bytes[2] << 16;
	cmd |= bytes[3] << 24;

#line 355 "disassembler.md"

	if ((cmd & 0xfe00707f) ==
		0x00005033
	) {
		
#line 371 "disassembler.md"

	write_reg((cmd & 0x00000f80) >> 7);
	put(" <- ");
	write_reg((cmd & 0x000f8000) >> 15);
	put(" >> ");
	write_reg((cmd & 0x01f00000) >> 20);

#line 359 "disassembler.md"

	}
	
#line 394 "disassembler.md"

	else if ((cmd & 0x0000707f) ==
		0x00007013) {
		write_reg(
			(cmd & 0x00000f80) >> 7
		);
		put(" <- ");
		write_reg(
			(cmd & 0x000f8000) >> 14
		);
		put(" and $");
		write_hex_int(
			(cmd & 0xfff00000) >> 20, -1
		);
	}

#line 414 "disassembler.md"

	else if ((cmd & 0x0000007f) ==
		0x00000037) {
		write_reg(
			(cmd & 0x00000f80) >> 7
		);
		put(" <- $");
		write_addr(cmd & 0xfffff000);
	}

#line 428 "disassembler.md"

	else if ((cmd & 0x0000007f) ==
		0x00000017) {
		write_reg(
			(cmd & 0x00000f80) >> 7
		);
		put(" <- %pc ");
		ulong target;
		if (cmd & 0x80000000) {
			put("- $");
			write_addr(-(cmd & 0x7ffff000) & 0x7ffff000);
			target = addr - (cmd & 0x7ffff000);
		} else {
			put("+ $");
			write_addr(cmd & 0x7ffff000);
			target = addr + (cmd & 0x7ffff000);
		}
		put(" // $");
		write_addr(target);
	}

#line 453 "disassembler.md"

	else if ((cmd & 0x0000707f) ==
		0x00000067) {
		write_reg(
			(cmd & 0x00000f80) >> 7
		);
		put(", %pc <- %pc + $04, %pc ");
		ulong target;
		if (cmd & 0x80000000) {
			put("- $");
			write_hex_int(-(cmd >> 20) & 0x3ff, -1);
			target = addr - ((cmd >> 20) & 0x3ff);
		} else {
			put("+ $");
			write_hex_int((cmd >> 20) & 0x3ff, -1);
			target = addr + ((cmd >> 20) & 0x3ff);
		}
		put(" // $");
		write_addr(target);
	}

#line 361 "disassembler.md"

	else {
		put("db.w $");
		write_hex_int(cmd, 4);
	}

#line 161 "disassembler.md"

		addr += 4;
	} else {
		
#line 382 "disassembler.md"

	write_hex_bytes(bytes, 2);
	put("       ");
	put("db.b $");
	write_hex_byte(bytes[0]);
	put(" $");
	write_hex_byte(bytes[1]);

#line 164 "disassembler.md"

		addr += 2;
	}
	putnl();

#line 919 "index.md"

		continue;
	}

#line 110 "index.md"

		
#line 122 "index.md"

	put("\aunknown command ");
	put(isprint(cmd) ? (char) cmd : '?');
	put(" (");
	write_hex_byte(cmd & 0xff);
	put(')'); putnl();

#line 111 "index.md"

	}
	put("quit"); putnl();

#line 10 "index.md"

	}
