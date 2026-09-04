"""Generate libogg's static slicing-by-8 CRC lookup table."""

def _hex32(value):
    digits = "0123456789abcdef"
    return "0x" + "".join([digits[(value >> shift) & 0xf] for shift in range(28, -1, -4)])

def _ogg_crc_table_impl(ctx):
    polynomial = 0x04c11db7
    table = []
    first = []
    for value in range(256):
        crc = value << 24
        for _ in range(8):
            crc = ((crc << 1) ^ (polynomial if crc & 0x80000000 else 0)) & 0xffffffff
        first.append(crc)
    table.append(first)

    for row_index in range(1, 8):
        previous = table[row_index - 1]
        row = []
        for value in range(256):
            crc = previous[value]
            row.append((first[(crc >> 24) & 0xff] ^ (crc << 8)) & 0xffffffff)
        table.append(row)

    lines = ["/* Generated from libogg's CRC polynomial. */", "static const ogg_uint32_t crc_lookup[8][256] = {"]
    for row in table:
        lines.append("  {")
        for offset in range(0, 256, 8):
            values = [_hex32(value) for value in row[offset:offset + 8]]
            lines.append("    " + ", ".join(values) + ",")
        lines.append("  },")
    lines.append("};")
    ctx.actions.write(ctx.outputs.out, "\n".join(lines) + "\n")

ogg_crc_table = rule(
    implementation = _ogg_crc_table_impl,
    attrs = {
        "out": attr.output(mandatory = True),
    },
)
