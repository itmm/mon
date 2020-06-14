# Monitor program
* monitor program for HiFive1 Rev B01
* runs also under Unix for testing purposes

```
@Def(file: mon.cpp)
	@put(includes)
	@put(globals)
	int main() {
		@put(main)
	}
@End(file: mon.cpp)
```
* general program structure

```
@inc(io.md)
```
* low-level I/O is defined in separate slide-set

```
@add(globals)
	using ulong = unsigned long;
	using uchr = unsigned char;
@end(globals)
```
* shorthand for often used types

## Start Address
* define start address for the monitor on the different platforms

```
@def(main)
	@put(init terminal)
	#if UNIX_APP
		@put(unix start)
	#else
		@put(riscv start)
	#endif
	ulong addr { start };
@end(main)
```
* initialize terminal and define start address
* start address is defined differently on Unix and RISC-V
* current address is set to start address

```
@def(unix start)
	char buffer[] = ""
		@put(initial buffer);
	const ulong start {
		reinterpret_cast<ulong>(
			buffer
		)
	};
@end(unix start)
```
* on Unix the start address points to a buffer
* that contains RISC-V object code
* to test the disassembler

```
@def(initial buffer)
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
@end(initial buffer)
```
* first bytes of the `@f(write_addr)` RISC-V implementation

```
@add(globals)
	@put(needed by write addr)
	static void write_addr(ulong addr) {
		@put(write addr)
	}
@end(globals)
```
* write a memory address in hexadecimal base

```
@def(riscv start)
	const ulong start {
		reinterpret_cast<ulong>(
			write_addr
		)
	};
@end(riscv start)
```
* on RISC-V the real implementation is used as start address

## Main loop
* main input loop for the monitor

```
@add(main)
	int cmd { 0 };
	while (cmd != EOF) {
		write_addr(addr);
		put("> ");
		cmd = get();
		if (cmd == EOF) { break; }
		@put(command switch)
		@mul(unknown cmd)
	}
	put("quit"); putnl();
@end(main)
```
* current address is printed as a cursor
* process the commands entered
* the fallback is a message for unknown commands
* quitting the monitor is only possible in the Unix version

```
@def(unknown cmd)
	put("\aunknown command ");
	put(isprint(cmd) ? (char) cmd : '?');
	put(" (");
	write_hex_byte(cmd & 0xff);
	put(')'); putnl();
@end(unknown cmd)
```
* errors and warnings are presented with a bell
* command and ASCII code (in hex) are printed

## Write hexadecimal address
* writes a memory address to the terminal

```
@def(needed by write addr)
	@put(needed by write hex byte)
	static void write_hex_byte(int byte) {
		@put(write hex byte)
	}
@end(needed by write addr)
```
* address is written as sequence of bytes

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
* all bytes of the address are written

```
@def(needed by write hex byte)
	static void write_hex_nibble(
		int nibble
	) {
		@put(write hex nibble)
	}
@end(needed by write hex byte)
```
* writing of a nibble is needed to write a byte

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
* writing a byte by writing its two nibbles
* bytes out of range written as `??`

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
* nibbles are written by index lookup

## Main loop backspace handling
* handle backspaces in the main loop

```
@def(command switch)
	while (cmd == 0x7f) {
		put('\a'); cmd = get();
	}
@end(command switch)
```
* a backspace in the main loop is an error
* the bell character is raised for each one
* but no error message is written
* to avoid cluttering the interface

## Dump memory
* dump regions of memory

```
@add(globals)
	constexpr int bytes_per_row { 16 };
	constexpr int default_rows { 8 };
	void dump_hex(ulong from, ulong to) {
		@put(dump hex)
	}
@end(globals)
```
* the number of bytes per row and default number of rows are hard coded
* the function `@f(dump_hex)` dumps the specified region

```
@add(command switch)
	if (cmd == '\n') {
		ulong from { addr };
		addr += default_rows *
			bytes_per_row;
		dump_hex(from, addr);
		continue;
	}
@end(command switch)
```
* a simple return press dumps the next default region of memory
* and increases the current address
* the `continue` is important
* otherwise the command will be treated as unknown

```
@def(dump hex)
	@mul(clear whole line)
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
* the current line is cleared
* and each line is dumped
* by clearing the line multilple return commands can provide
  continuous dumps

```
@def(clear whole line)
	put("\x1b[G\x1b[K");
@end(clear whole line)
```
* the first escape sequence moves the cursor into the first column
* the second sequence earses the whole line

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
* each line consists of the address
* followed by `:` instead of `>`
* and the bytes in hexadecimal and ascii notation

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
* the line is filled with spaces if not enough bytes for a whole line
  are present
* the are padded in groups of 8

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
* only printable characters are written in the ASCII part
* also missing characters are written as spaces
* and the same padding is applied as in the hexadecimal block

```
@def(pad dump)
	if (row + 1 != bytes_per_row &&
		row % 8 == 7
	) { put(' '); }
@end(pad dump)
```
* add an additional space every 8 bytes

## Terminal setup
* initializes the terminal

```
@add(includes)
	#if UNIX_APP
		#include <termios.h>
		#include <unistd.h>
	#endif
@end(includes)
```
* under Unix these headers are needed to switch the terminal into
  raw mode

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
* a class handles the terminal switching under Linux
* to the terminal can be switched back automatically on destruction

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
* do not echo characters and use the raw mode
* safe the original 

```
@def(reset term handler)
	tcsetattr(STDIN_FILENO, TCSAFLUSH,
		&orig_
	);
@end(reset term handler)
```
* restore original mode

```
@def(init terminal) 
	#if UNIX_APP
		Term_Handler term_handler;
	#else
		@put(setup riscv terminal)
	#endif
@end(init terminal)
```
* different setup under Linux and RISC-V

```
@add(globals)
	#if ! UNIX_APP
		volatile int *prci {
			reinterpret_cast<
				volatile int *
			>(0x10008000) };
	#endif
@end(globals)
```
* base for the clock registers

```
@def(setup riscv terminal)
	constexpr int hfrosccfg { 0x00/4 };
	constexpr int hfxosccfg { 0x04/4 };
	while (prci[hfxosccfg] >= 0) { }
	prci[hfrosccfg] &= ~0x40000000;
@end(setup riscv terminal)
```
* wait for external clock to become ready
* and disable internal clock afterwards

```
@add(setup riscv terminal)
	constexpr int tx_control { 0x08/4 };
	uart[tx_control] |= 1;
	constexpr int rx_control { 0x0c/4 };
	uart[rx_control] |= 1;
@end(setup riscv terminal)
```
* enable UART reading and writing

```
@add(setup riscv terminal)
	constexpr int div { 0x18/4 };
	uart[div] =
		(uart[div] & ~0xffff) | 139;
@end(setup riscv terminal)
```
* set the correct divider for 115200 baud rate

```
@add(setup riscv terminal)
	constexpr int rx_data { 0x01 };
	while (uart[rx_data] >= 0) {}
@end(setup riscv terminal)
```
* discard anything in the input buffer

## More complex memory dump command
* the `m` memory command allows to specify a start and end address

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
@end(globals)
```

```
@add(globals)
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
			--digits; value = value >> 4;
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
	void write_int(unsigned int v) {
		if (v >= 10) {
			write_int(v / 10);
		}
		put((v % 10) + '0');
	}
@end(needed by addr get)
```
* writes a decimal number
* needed for writing escape sequences

```
@add(needed by addr get)
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
	MC_State state {
		MC_State::before_enter_1st
	};
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
		@put(memory command cases)
	} }
@end(memory command)
```

```
@def(memory command cases)
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
@end(memory command cases)
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
		if (cmd == '.') { cmd = get(); }
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
			put('\a'); cmd = get();
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

## More commands

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
@inc(disassembler.md)
```

