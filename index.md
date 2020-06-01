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
		@put(command switch)
		put("unknown command\n");
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
@end(write addr)
```

```
@def(command switch)
	if (cmd == '\n' || cmd == '\r') {
		@put(dump hex)
		continue;
	}
@end(command switch)
```

```
@def(dump hex)
	for (
		int lines { 8 }; lines; --lines
	) {
		constexpr int bytes_per_row {
			16
		};
		write_addr(addr);
		put(": ");
		@put(dump hex part)
		put("| ");
		@put(dump text part)
		put('\n');
		addr += bytes_per_row;
	}
@end(dump hex)
```

```
@def(dump hex part) {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		write_hex_byte(addr[row] & 0xff);
		put(' ');
		@mul(pad dump)
	}
} @end(dump hex part)
```

```
@def(dump text part) {
	int row { 0 };
	for (; row < bytes_per_row; ++row) {
		if (isprint(addr[row])) {
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
