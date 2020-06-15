# Low Level Input/Output
* the I/O routines wrap the C++ standard I/O on Unix
* and use the UART on RISC-V

```
@Def(includes)
	#if UNIX_APP
		#include <iostream>
	#else
		@put(replace library)
	#endif
@end(includes)
```
* use the standard library on Unix
* mock some standard identifiers on RISC-V

```
@def(replace library)
	constexpr int EOF { -1 };
	inline bool isprint(char ch) {
		return ch >= ' ' && ch <= '~';
	}
@end(replace library)
```
* end of file marker and checking for printable characters are
  reimplemented
* the original version depends on too many other stuff

```
@Def(globals)
	@put(needed by put)
	void put(char ch) {
		@put(put)
	}
@End(globals)
```
* write a single character

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
* memory address of the UART configuration on RISC-V

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
* Unix uses standard output
* RISC-V waits until data can be written
* and writes the character

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
* to put a string, single characters are written
* one at a time

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
* newline is treated differently on Unix and RISC-V

```
@Add(globals)
	int get() {
		@put(get)
	}
@end(globals)
```
* reads a character

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
* uses standard input on Unix
* CTRL-D is converted to end of file
* on RISC-V it polls the hardware until a character is available
* if the character is a return it is converted to a newline

```
@def(wait for get ready)
	while (res < 0) {
		res = uart[rx_data];
	}
	res = res & 0xff;
@end(wait for get ready)
```
* poll hardware until a character is available

