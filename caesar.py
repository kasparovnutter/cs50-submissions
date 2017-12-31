import sys, argparse

def main(argv):
    # "".join() = str, concat str/list values with "" seperator
    # for x in sys.argv[1:]:
    #   if "-" not in x:        // filters negative values (if "-" present anywhere in argv[x:])
    #       for x in x:
    #           if x.isdigit(): //this also gets rid of negative signs ("-")
    #               str(x)

    test = "".join(str(x) for x in sys.argv[1:] if "-" not in x for x in x if x.isdigit())

    parser = argparse.ArgumentParser\
    (usage="%(prog)s [-h] [key (any int)] [-sum]")
    parser.add_argument("key", nargs = "*", type = int)
    parser.add_argument("-sum", action="store_true", help="digital sum of every inputted number found")
    args, unknown= parser.parse_known_args()

    while True:
        if len(sys.argv) == 2 and argv[1].isdigit() and int(argv[1]) > 0:
            key = int(sys.argv[1])
            break
        elif test.isdigit():
            if args.sum == True:
                key = sum(int(x) for x in test)
                break
            key = int(test)
            break
        else:
            #try:
                #key = int(input("\033[1F\033[K"+ "retry:"))
            #except NameError and ValueError:
                #continue
            #if key <= 0:
                #continue
            #break
            parser.print_help()
            sys.exit(2)


    print(key)
    cipher = caesar(key,input("plaintext:"))
    print("ciphertext:{}".format(cipher))
    sys.exit(0)

def caesar(key,plain):
    key %= 26
    c = ""
    for x in plain:
        if x.isalpha():
            # chr(i) = returns str of int i (ascii)
            # ord(str) = returns int of str (ascii)
            # [(x + key) % last letter] % z % a + a
            #

            # %last letter gets rid of overshoots (eg Z + 1 = "[")
            # (e.g) "Z" = 90, key = 6
            # "Z" + 6 = 96 = "`"
            # 96 % 91 = 5
            # 5 % 65 = 5
            # 5 + 65 = 70 = "F"

            # %first letter for the rest (eg B + 1 = 67, 67 % Z = 67)
            # "A" + 6 = 71
            # 71 % 90 = 71
            # 71 % 65 = 6
            # 6 + 65 = 71

            # + first letter to add everything back to ascii
            if x.islower():
                c+= chr((ord(x) + key) % (ord('z') + 1) % ord('a') + ord('a'))
            elif x.isupper():
                c+= chr((ord(x) + key) % (ord('Z') + 1) % ord('A') + ord('A'))
        else:
            c += x

    return(c)
    #print(key, "{}\n{}".format(plain,c))

if __name__ == "__main__":
    main(sys.argv)

