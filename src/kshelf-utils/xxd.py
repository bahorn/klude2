#!/usr/bin/env python3
"""
reimplementation of -i from xxd, with -n support.

as -n is new, and older ubuntu doesn't have it.
"""
import sys

source = sys.argv[1]
name = sys.argv[2]


data = open(source, 'rb').read()
body = ', '.join(map(lambda x: hex(x), bytes(data)))

template = f'''
unsigned char {name}[] = {{
    {body}
}};

unsigned int {name}_len = {len(data)};
'''
print(template)
