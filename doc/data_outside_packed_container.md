# Data outside packed container

## Passwords

- ROM offs 787B: contains array of `{level def, pass}` entries.
  Ended with dummy entry `PASS` leading to level def number `0xFFFF`.

Passwords are non-present in any other form - `STRT` decodes to `43 44 42 44` hex, the sequence isn't present in any other file in any endian order.
