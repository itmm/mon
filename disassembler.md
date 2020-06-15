# Disassembler
* disassembles an instruction

```
@Add(globals)
	void write_hex_int(
		int value, int bytes
	) {
		@put(write hex int)
	}
@End(globals)
```
* writes a hexadecimal `value`
* only the lowest `bytes` of `value` are written
* if `bytes` is negative, enough bytes will be written to fit the
  value

```
@def(write hex int)
	if (bytes < 0) {
		@put(write dynamic hex int)
		return;
	}
@end(write hex int)
```
* write dynamic number of bytes

```
@add(write hex int)
	for (; bytes; --bytes) {
		write_hex_byte(
			(value >> (8 * (bytes - 1))) &
			0xff
		);
	}
@end(write hex int)
```
* write as many hexadecimal bytes as specified

```
@def(write dynamic hex int)
	if (value < 256) {
		write_hex_int(value, 1);
	} else if (value < 0x10000) {
		write_hex_int(value, 2);
	} else {
		write_hex_int(value, 4);
	}
@end(write dynamic hex int)
```
* write `1`, `2` or `4` bytes, depending on `value`

```
@Add(globals)
	void write_reg(int reg) {
		switch (reg) {
			@put(write reg cases)
			default:
				put("?? # ");
				write_int(reg);
		}
	}
@End(globals)
```
* write the name of a register

```
@def(write reg cases)
	case 0: put("%zero"); break;
	case 1: put("%ra"); break;
	case 2: put("%sp"); break;
	case 3: put("%gp"); break;
	case 4: put("%tp"); break;
@end(write reg cases)
```
* write named registers

```
@add(write reg cases)
	case 5: case 6: case 7:
		put("%t");
		put('0' + reg - 5);
		break;
@end(write reg cases)
```
* first temporary registers

```
@add(write reg cases)
	case 8: case 9:
		put("%s");
		put('0' + reg - 8);
		break;
@end(write reg cases)
```
* first saved registers

```
@add(write reg cases)
	case 10: case 11: case 12:
	case 13: case 14: case 15:
	case 16: case 17:
		put("%a");
		put('0' + reg - 10);
		break;
@end(write reg cases)
```
* argument registers

```
@add(write reg cases)
	case 18: case 19: case 20:
	case 21: case 22: case 23:
	case 24: case 25: case 26:
	case 27:
		put("%s");
		write_int(reg - 18 + 2);
		break;
@end(write reg cases)
```
* second saved registers

```
@add(write reg cases)
	case 28: case 29: case 30:
	case 31:
		put("%t");
		put('0' + reg - 31 + 3);
		break;
@end(write reg cases)
```
* second temporary registers

```
@Def(disassemble)
	@Mul(clear whole line)
	write_addr(addr);
	put("  ");
@End(disassemble)
```
* clear line and write address

```
@Add(disassemble)
	const auto bytes { reinterpret_cast<
		uchr *
	>(addr) };
	unsigned cmd { 0 };
	cmd = bytes[0];
@End(disassemble)
```
* get byte pointer to address
* and the value of the first byte

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
* size of the command depends on the first bits of the first byte

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
* write multiple hexadecimal bytes

```
@def(disassemble 16 bit)
	write_hex_bytes(bytes, 2);
	put("       ");
	cmd |= bytes[1] << 8;
@end(disassemble 16 bit)
```
* write two bytes
* and build two-byte command

```
@add(disassemble 16 bit)
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
* disassemble two-byte command

```
@def(16 bit cases)
	else if ((cmd & 0xe003) == 0x0001 &&
		(cmd & 0x0f80)
	) {
		@put(add immediate 16)
	}
@end(16 bit cases)
```
* disassemble `reg <- reg + immediate`

```
@def(add immediate 16)
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
@end(add immediate 16)
```
* extract register and immediate value
* handle negative immediate by negating the value

```
@add(16 bit cases)
	else if ((cmd & 0xe003) == 0x4001 &&
		(cmd & 0x0f80)
	) {
		@put(set immediate 16)
	}
@end(16 bit cases)
```
* disassemble `reg <- immediate`

```
@def(set immediate 16)
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
@end(set immediate 16)
```
* extract register and immediate value
* handle negative immediate by negating the value

```
@add(16 bit cases)
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
@end(16 bit cases)
```
* disassemble `[%sp + immediate] <- reg`

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
		put(" <- [%sp + $");
		write_hex_int(offset, -1);
		put(']');
	}
@end(16 bit cases)
```
* disassemble `reg <- [%sp + immediate]`

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
* disassemble `reg <- reg`

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
* disassemble `reg <- reg + reg`

```
@add(16 bit cases)
	else if (cmd == 0x8082) {
		put("%pc <- %ra // return");
	}
@end(16 bit cases)
```
* disassemble `return`

```
@def(disassemble 32 bit)
	write_hex_bytes(bytes, 4);
	put(' ');
	cmd |= bytes[1] << 8;
	cmd |= bytes[2] << 16;
	cmd |= bytes[3] << 24;
@end(disassemble 32 bit)
```
* write four bytes
* and generate full command

```
@add(disassemble 32 bit)
	if ((cmd & 0xfe00707f) ==
		0x00005033
	) {
		@put(shift right)
	}
	@put(32 bit cases)
	else {
		put("db.w $");
		write_hex_int(cmd, 4);
	}
@end(disassemble 32 bit)
```
* disassemble 32 bit commands

```
@def(shift right)
	write_reg((cmd & 0x00000f80) >> 7);
	put(" <- ");
	write_reg((cmd & 0x000f8000) >> 15);
	put(" >> ");
	write_reg((cmd & 0x01f00000) >> 20);
@end(shift right)
```
* disassemble `reg <- reg >> reg`

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
* write unknown 32 bit sequence

```
@def(32 bit cases)
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
@end(32 bit cases)
```
* disassemble `reg <- reg and immediate`

```
@add(32 bit cases)
	else if ((cmd & 0x0000007f) ==
		0x00000037) {
		write_reg(
			(cmd & 0x00000f80) >> 7
		);
		put(" <- $");
		write_addr(cmd & 0xfffff000);
	}
@end(32 bit cases)
```
* disassemble `reg <- immediate`

```
@add(32 bit cases)
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
@end(32 bit cases)
```
* disassemble `reg <- %pc + immediate`

```
@add(32 bit cases)
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
@end(32 bit cases)
```
* disassemble `reg, %pc <- %pc + $04, %pc + immediate`
