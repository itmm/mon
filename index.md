# Monitor program

```
@Def(file: mon.cpp)
	@put(includes)
	@put(globals)
	int main() {
		@put(main)
	}
@End(file: mon.cpp)
```

```
@inc(io.md)
```

```
@add(globals)
	using ulong = unsigned long;
	@put(needed by write addr)
	static void write_addr(ulong addr) {
		@put(write addr)
	}
@end(globals)
```

```
@def(main)
	@put(init terminal)
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
@end(main)
```

```
@add(main)
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
@end(main)
```

```
@add(main)
	while (cmd != EOF) {
		write_addr(addr);
		put("> ");
		cmd = get();
		if (cmd == EOF) { break; }
		while (cmd == 0x7f) {
			put('\a');
			cmd = get();
		}
		@put(command switch)
		@mul(unknown cmd)
	}
	put("quit"); putnl();
@end(main)
```

```
@def(unknown cmd)
	put("\aunknown command ");
	put(isprint(cmd) ? (char) cmd : '?');
	put(" (");
	write_hex_byte(cmd & 0xff);
	put(')'); putnl();
@end(unknown cmd)
```

```
@def(needed by write addr)
	@put(needed by write hex byte)
	static void write_hex_byte(int byte) {
		@put(write hex byte)
	}
@end(needed by write addr)
```

```
@def(needed by write hex byte)
	static void write_hex_nibble(
		int nibble
	) {
		@put(write hex nibble)
	}
@end(needed by write hex byte)
```

```
@def(write hex nibble)
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
@end(write hex nibble)
```

```
@def(write hex byte)
	if (byte >= 0 && byte <= 255) {
		write_hex_nibble(byte >> 4);
		write_hex_nibble(byte & 0xf);
	} else {
		put("??");
	}
@end(write hex byte)
```

```
@def(write addr)
	int shift { (sizeof(ulong) - 1) * 8 };
	for (; shift >= 0; shift -= 8) {
		write_hex_byte(
			(addr >> shift) & 0xff
		);
	}
@end(write addr)
```

```
@add(globals)
	constexpr int bytes_per_row { 16 };
	constexpr int default_rows { 8 };
	void dump_hex(ulong from, ulong to) {
		@put(dump hex)
	}
@end(globals)
```

```
@def(command switch)
	if (cmd == '\n') {
		ulong from { addr };
		addr += default_rows *
			bytes_per_row;
		dump_hex(from, addr);
		continue;
	}
@end(command switch)
```

```
@def(dump hex)
	put("\x1b[0E\x1b[2K");
	if (from && from < to) {
		for (
			; from < to;
			from += bytes_per_row
		) {
			@put(dump hex line)
			putnl();
		}
	}
@end(dump hex)
```

```
@def(dump hex line)
	write_addr(from);
	put(": ");
	auto addr {
		reinterpret_cast<const char *>(
			from
		)
	};
	@put(dump hex part)
	put("| ");
	@put(dump text part)
@end(dump hex line)
```

```
@def(dump hex part) {
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
		@mul(pad dump)
	}
} @end(dump hex part)
```

```
@def(dump text part) {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		if (from + row >= to) {
			put(' ');
		} else if (isprint(addr[row])) {
			put(addr[row]);
		} else {
			put('.');
		}
		@mul(pad dump)
	}
} @end(dump text part)
```

```
@def(pad dump)
	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }
@end(pad dump)
```

```
@add(includes)
	#if UNIX_APP
		#include <termios.h>
		#include <unistd.h>
	#endif
@end(includes)
```

```
@add(globals)
	#if UNIX_APP
		class Term_Handler {
			termios orig_;
		public:
			Term_Handler() {
				@put(init term handler)
			}
			~Term_Handler() {
				@put(reset term handler)
			}
		};
	#endif
@end(globals)
```

```
@def(init term handler)
	tcgetattr(STDIN_FILENO, &orig_);
	termios raw { orig_ };
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(
		STDIN_FILENO, TCSAFLUSH, &raw
	);
@end(init term handler)
```

```
@def(reset term handler)
	tcsetattr(STDIN_FILENO, TCSAFLUSH,
		&orig_
	);
@end(reset term handler)
```

```
@def(init terminal) 
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
@end(init terminal)
```

```
@add(globals)
	void write_int(unsigned int v) {
		if (v >= 10) {
			write_int(v / 10);
		}
		put((v % 10) + '0');
	}
@end(globals)
```

```
@add(globals)
	struct Addr_State {
		ulong ref;
		ulong dflt;
		bool relative { false };
		bool negative { false };
		ulong value { 0 };
		unsigned digits { 0 };
		ulong get(int &cmd);
	};
	@put(needed by addr get)
	ulong Addr_State::get(int &cmd) {
		@put(get addr)
	}
@end(globals)
```

```
@def(get addr)
	bool done { false };
	for (;;) {
		switch (cmd) {
		@put(get addr chars)
		default:
			done = true;
			break;
		}
		if (done) { break; }
		cmd = ::get();
	}
@end(get addr)
```

```
@def(get addr chars)
	case 0x7f:
		if (! digits) {
			if (! relative) {
				return 0;
			}
			@mul(delete after prev char)
			relative = negative = false;
		} else {
			@mul(delete after prev char)
			--digits;
			value = value >> 4;
		}
		break;
@end(get addr chars)
```

```
@def(delete after prev char)
	put("\x1b[D\x1b[K");
@end(delete after prev char)
```

```
@add(get addr chars)
	case '+': case '-':
		if (digits || relative) {
			done = true; break;
		}
		put(static_cast<char>(cmd));
		relative = true;
		negative = (cmd == '-');
		break;
@end(get addr chars)
```

```
@add(get addr chars)
	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9': case 'a': case 'b':
	case 'c': case 'd': case 'e':
	case 'f': {
		@put(get addr digit)
		break;
	}
@end(get addr chars)
```

```
@def(needed by addr get)
	void delete_after_prev_chars(int n) {
		if (n > 0) {
			put("\x1b[");
			write_int(n);
			put("D\x1b[K");
		}
	}
@end(needed by addr get)
```

```
@def(get addr digit)
	if (! value) {
		delete_after_prev_chars(digits);
		digits = 0;
	}
@end(get addr digit)
```
* clear leading zeros

```
@add(get addr digit)
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
@end(get addr digit)
```

```
@add(get addr)
	if (! relative) {
		@put(addr not relative)
	} else if (negative) {
		@put(addr neg relative)
	} else {
		@put(addr pos relative)
	}
@end(get addr)
```

```
@def(addr not relative)
	if (! digits) {
		value = dflt;
	}
@end(addr not relative)
```

```
@def(addr neg relative)
	if (value > ref) {
		put('\a');
		value = 0;
	} else {
		value = ref - value;
	}
@end(addr neg relative)
```

```
@def(addr pos relative)
	constexpr ulong mx { ~0ul };
	if (mx - value < ref) {
		put('\a');
		value = mx;
	} else {
		value = ref + value;
	}
@end(addr pos relative)
```

```
@add(get addr)
	delete_after_prev_chars(
		digits + (relative ? 1 : 0)
	);
	write_addr(value);
	relative = negative = false;
	digits = sizeof(ulong) * 2;
	return value;
@end(get addr)
```

```
@add(command switch)
	if (cmd == 'm') {
		@put(memory command)
		continue;
	}
@end(command switch)
```

```
@add(globals)
	enum class MC_State {
		before_enter_1st,
		entering_1st,
		after_entering_1st,
		entering_2nd
	};
@end(globals)
```

```
@def(memory command)
	MC_State state { MC_State::before_enter_1st };
	put("memory ");
	cmd = get();
	Addr_State from;
	Addr_State to;
	from.ref = addr;
	from.dflt = addr;
@end(memory command)
```

```
@add(memory command)
	bool done { false };
	while (! done) { switch (state) {
		case MC_State::before_enter_1st:
			@put(mc before enter 1st)
			break;
		case MC_State::entering_1st:
			@put(mc entering 1st)
			break;
		case MC_State::after_entering_1st:
			@put(mc after entering 1st)
			break;
		case MC_State::entering_2nd:
			@put(mc entering 2nd)
			break;
	} }
@end(memory command)
```

```
@def(clear whole line)
	put("\x1b[G\x1b[K");
@end(clear whole line)
```

```
@def(mc before enter 1st)
	if (cmd == 0x7f) {
		@mul(clear whole line)
		done = true;
	} else {
		state = MC_State::entering_1st;
	}
@end(mc before enter 1st)
```

```
@def(mc entering 1st)
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
@end(mc entering 1st)
```

```
@def(mc after entering 1st) {
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
} @end(mc after entering 1st)
```

```
@def(mc entering 2nd)
	to.get(cmd);
	if (cmd == 0x7f) {
		state =
			MC_State::after_entering_1st;
	} else {
		if (cmd == '\n') {
			@put(mc done)
			done = true;
		} else {
			put('\a');
			cmd = get();
		}
	}
@end(mc entering 2nd)
```

```
@def(mc done)
	if (from.value <= to.value) {
		putnl();
		dump_hex(from.value, to.value);
		addr = to.value;
	} else {
		put('\a'); putnl();
	}
@end(mc done)
```

```
@add(command switch)
	if (cmd == 'x') {
		put("reset to start"); putnl();
		addr = start;
		continue;
	}
@end(command switch)
```

```
@add(command switch)
	if (cmd == 'd') {
		@put(disassemble)
		continue;
	}
@end(command switch)
```

```
@add(globals)
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
@end(globals)
```

```
@add(globals)
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
@end(globals)
```

```
@def(disassemble)
	@mul(clear whole line)
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
@end(disassemble)
```
