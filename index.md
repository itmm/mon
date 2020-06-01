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
	char *addr {
		reinterpret_cast<char *>(write_addr)
	};
	for (;;) {
		write_addr(addr);
		std::cout << "> ";
		std::cout << "unknown command\n";
		break;
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
	static void write_hex_nibble(int nibble) {
		static const char digits[] = "0123456789abcdef";
		static_assert(sizeof(digits) == 16 + 1);
		if (nibble >= 0 && nibble <= 15) {
			std::cout << digits[nibble];
		} else {
			std::cout << '?';
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
		std::cout << "??";
	}
@end(write hex byte)
```

```
@def(write addr)
	unsigned long value { reinterpret_cast<unsigned long>(addr) };
	std::cout << '$';
	int shift { (sizeof(long) - 1) * 8 };
	for (; shift >= 0; shift -= 8) {
		write_hex_byte((value >> shift) & 0xff);
	}
@end(write addr)
```
