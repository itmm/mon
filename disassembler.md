# Disassembler

```
@Def(disassemble)
	@Mul(clear whole line)
	write_addr(addr);
	put("  ");
	const auto bytes { reinterpret_cast<
		uchr *
	>(addr) };
	unsigned cmd { 0 };
	cmd = bytes[0];
@End(disassemble)
```

```
@Add(disassemble)
	if ((cmd & 0x3) != 0x3) {
		@put(disassemble 16 bit)
		addr += 2;
	} else if ((cmd & 0xc) != 0xc) {
		@put(disassemble 32 bit)
		addr += 4;
	} else {
		@put(disassemble unknown)
		addr += 2;
	}
	putnl();
@End(disassemble)
```

```
@Add(globals)
	void write_hex_bytes(
		const uchr *bytes,
		int count
	) {
		for (; count; --count, ++bytes) {
			write_hex_byte(*bytes);
			put(' ');
		}
	}
@End(globals)
```

```
@def(disassemble 16 bit)
	write_hex_bytes(bytes, 2);
	put("       ");
	cmd |= bytes[1] << 8;
	if (cmd == 0x0000) {
		put("db.s $0000 // illegal");
	}
	@put(16 bit cases)
	else {
		put("db.s $");
		write_hex_int(cmd, 2);
	}
@end(disassemble 16 bit)
```

```
@def(16 bit cases)
	else if ((cmd & 0xe003) == 0x0001 &&
		(cmd & 0x0f80)
	) {
		@put(add immediate 16)
	}
@end(16 bit cases)
```

```
@def(add immediate 16)
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
@end(add immediate 16)
```

```
@add(16 bit cases)
	else if ((cmd & 0xe003) == 0x4001 &&
		(cmd & 0x0f80)
	) {
		@put(set immediate 16)
	}
@end(16 bit cases)
```

```
@def(set immediate 16)
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
@end(set immediate 16)
```

```
@add(16 bit cases)
	else if ((cmd & 0xe003) == 0xc002) {
		auto reg { (cmd & 0x7c) >> 2 };
		auto offset {
			((cmd & 0x1e00) >> (9 - 2)) |
			((cmd & 0x0180) >> (7 - 6))
		};
		put("[sp + $");
		write_hex_int(offset, -1);
		put("] <- ");
		write_reg(reg);
	}
@end(16 bit cases)
```

```
@add(16 bit cases)
	else if ((cmd & 0xe003) == 0x4002) {
		auto reg { (cmd & 0x0f80) >> 7 };
		auto offset {
			((cmd & 0x0070) >> (4 - 2)) |
			((cmd & 0x000c) << (6 - 2)) |
			((cmd & 0x1000) >> (12 - 5))
		};
		write_reg(reg);
		put(" <- [sp + $");
		write_hex_int(offset, -1);
		put(']');
	}
@end(16 bit cases)
```

```
@add(16 bit cases)
	else if ((cmd & 0xf003) == 0x8002 &&
		(cmd & 0x007c)
	) {
		write_reg((cmd & 0x0f80) >> 7);
		put(" <- ");
		write_reg((cmd & 0x007c) >> 2);
	}
@end(16 bit cases)
```

```
@add(16 bit cases)
	else if ((cmd & 0xf003) == 0x9002 &&
		(cmd & 0x007c)
	) {
		write_reg((cmd & 0x0f80) >> 7);
		put(" <- ");
		write_reg((cmd & 0x0f80) >> 7);
		put(" + ");
		write_reg((cmd & 0x007c) >> 2);
	}
@end(16 bit cases)
```

```
@add(16 bit cases)
	else if (cmd == 0x8082) {
		put("ret");
	}
@end(16 bit cases)
```

```
@def(disassemble 32 bit)
	write_hex_bytes(bytes, 4);
	put(' ');
	cmd |= bytes[1] << 8;
	cmd |= bytes[2] << 16;
	cmd |= bytes[3] << 24;
@end(disassemble 32 bit)
```

```
@add(disassemble 32 bit)
	if ((cmd & 0xfe00707f) ==
		0x00005033
	) {
		@put(shift right)
	}
	else {
		put("db.w $");
		write_hex_int(cmd, 4);
	}
@end(disassemble 32 bit)
```

```
@def(shift right)
	write_reg((cmd & 0x00000f80) >> 7);
	put(" <- ");
	write_reg((cmd & 0x000f8000) >> 15);
	put(" >> ");
	write_reg((cmd & 0x01f00000) >> 20);
@end(shift right)
```

```
@def(disassemble unknown)
	write_hex_bytes(bytes, 2);
	put("       ");
	put("db.b $");
	write_hex_byte(bytes[0]);
	put(" $");
	write_hex_byte(bytes[1]);
@end(disassemble unknown)
```
