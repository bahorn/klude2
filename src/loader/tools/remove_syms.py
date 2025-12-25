import sys
import subprocess


def run_command(command):
    result = subprocess.run(
        command,
        shell=isinstance(command, str),
        capture_output=True,
        text=True
    )
    return result.stdout, result.stderr


fname = sys.argv[1]

cmd_out, _ = run_command(
    f'readelf -T --syms {fname} | grep -e "FUNC" -e "OBJECT"'
)

all_syms = []

for line in cmd_out.split('\n'):
    sym = line.split(' ')[-1]
    if not sym:
        continue
    all_syms.append(f'{sym}')


with open(fname, 'rb') as f:
    to_patch = bytearray(f.read())


def find_all(data, pattern):
    positions = []
    start = 0

    while True:
        pos = data.find(pattern, start)
        if pos == -1:
            break
        positions.append((pos, len(pattern)))
        start = pos + 1

    return positions


all_pos = []
for sym in all_syms:
    all_pos += find_all(to_patch, bytes(sym, 'ascii'))

for pos, sze in all_pos:
    to_patch[pos:pos+sze] = b'\x00' * sze

with open(fname, 'wb') as f:
    f.write(to_patch)
