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
@def(includes)
	#include <iostream>
@end(includes)
```

```
@def(globals)
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
@end(globals)
```

```
@add(globals)
	@put(needed by write addr)
	static void write_addr(
		const char *addr
	) {
		@put(write addr)
	}
@end(globals)
```

```
@def(main)
	@put(init terminal)
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
		@put(command switch)
		put("unknown command ");
		put(isprint(cmd) ? (char) cmd : '?');
		put(" (");
		write_hex_byte(cmd & 0xff);
		put(")\n");
	}
@end(main)
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
@end(needed by write hex byte)
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
@end(write addr)
```

```
@add(globals)
	void dump_hex(const char *from,
		const char *to
	) {
		@put(dump hex)
	}
@end(globals)
```

```
@def(command switch)
	if (cmd == '\n' || cmd == '\r') {
		char *from { addr };
		addr += 8 * 16;
		dump_hex(from, addr);
		continue;
	}
@end(command switch)
```

```
@def(dump hex)
	put("\x1b[0E\x1b[2K");
	constexpr int bytes_per_row { 16 };
	if (from && from < to) {
		for (
			; from < to;
			from += bytes_per_row
		) {
			write_addr(from);
			put(": ");
			@put(dump hex part)
			put("| ");
			@put(dump text part)
			put('\n');
		}
	}
@end(dump hex)
```

```
@def(dump hex part) {
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
		} else if (isprint(from[row])) {
			put(from[row]);
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
	#include <termios.h>
	#include <unistd.h>
@end(includes)
```

```
@add(globals)
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
@end(globals)
```

```
@def(init terminal) 
	Term_Handler term_handler;
@end(init terminal)
```

