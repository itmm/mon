# Low Level Input/Output

```
@Def(includes)
	#if UNIX_APP
		#include <iostream>
	#else
		@put(replace library)
	#endif
@end(includes)
```

```
@def(replace library)
	constexpr int EOF { -1 };
	inline bool isprint(char ch) {
		return ch >= ' ' && ch <= '~';
	}
@end(replace library)
```

```
@Def(globals)
	@put(needed by put)
	void put(char ch) {
		@put(put)
	}
@End(globals)
```

```
@def(needed by put)
	#if ! UNIX_APP
		volatile int *uart {
			reinterpret_cast<
				volatile int *
			>(0x10013000) };
	#endif
@end(needed by put)
```

```
@def(put)
	#if UNIX_APP
		std::cout.put(ch);
	#else
		constexpr int tx_data { 0x00 };
		while (uart[tx_data] < 0) {}
		uart[tx_data] = ch;
	#endif
@end(put)
```

```
@Add(globals)
	void put(const char *begin) {
		if (begin) {
			while (*begin) {
				put(*begin++);
			}
		}
	}
@End(globals)
```

```
@Add(globals)
	void putnl() {
		#if UNIX_APP
			put('\n');
		#else
			put("\r\n");
		#endif
	}
@End(globals)
```

```
@Add(globals)
	int get() {
		@put(get)
	}
@end(globals)
```

```
@def(get)
	#if UNIX_APP
		int res { std::cin.get() };
		if (res == 0x04) { res = EOF; }
	#else
		constexpr int rx_data { 0x01 };
		int res { EOF };
		@put(wait for get ready)
		res = res & 0xff;
	#endif
	if (res == '\r') { res = '\n'; }
	return res;
@end(get)
```

```
@def(wait for get ready)
	while (res < 0) {
		res = uart[rx_data];
	}
	res = res & 0xff;
@end(wait for get ready)
```

