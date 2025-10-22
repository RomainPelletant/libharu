#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Recover one PDF from a serial log produced by a libharu sample."""

import argparse
from pathlib import Path


def extract_pdf(log):
    """Validate framing and byte count before returning the PDF bytes."""
    data = bytearray()
    expected = None
    for line in log.splitlines():
        line = line.strip()
        if line.startswith("PDF-BEGIN "):
            if expected is not None:
                raise ValueError("interrupted PDF transfer")
            _, _, length = line.split()
            expected = int(length)
            if expected <= 0:
                raise ValueError("invalid PDF size")
        elif line.startswith("PDF-DATA ") and expected is not None:
            data.extend(bytes.fromhex(line[len("PDF-DATA "):]))
            if len(data) > expected:
                raise ValueError("PDF exceeds announced size")
        elif line == "PDF-END" and expected is not None:
            if len(data) != expected:
                raise ValueError("incomplete PDF transfer")
            if not data.startswith(b"%PDF-") or not data.endswith(b"%%EOF\n"):
                raise ValueError("invalid PDF header or trailer")
            return bytes(data)
    raise ValueError("no complete PDF transfer found")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path, help="captured serial console log")
    parser.add_argument("output", type=Path, help="destination PDF (must not exist)")
    args = parser.parse_args()
    try:
        data = extract_pdf(args.log.read_text(encoding="utf-8", errors="replace"))
        with args.output.open("xb") as output:
            output.write(data)
    except (OSError, ValueError) as error:
        parser.exit(1, f"{error}\n")
    print(f"Saved {len(data)} bytes to {args.output}")


if __name__ == "__main__":
    main()
