import sys


def main():
    f = open(sys.argv[1])
    for line in f:
        if '[' in line:
            continue
        if '.' in line:
            continue
        address, symbol_type, symbol = line.strip().split(' ')
        print(f'void *{symbol} = 0;')


if __name__ == "__main__":
    main()
