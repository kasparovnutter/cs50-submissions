import argparse, sys



def main(argv):

    parser = argparse.ArgumentParser()
    addarg = parser.add_argument
    addarg("k", nargs="?", type = str, default = None, metavar="keyword (alphabetical)")


    # parse_args() = returns namespace <object> only
    # a = parse_args()
    # print(a)
    # Namespace(foo="bar")

    # parse_known_args() = returns 2-item tuple (namespace, list of unknown argument strings)
    # c = parse_known_args()
    # print(c)
    # (Namespace(foo="bar"), ["unknown argument 6969"])
    # a, b = parse_known_args()
    # print(a)
    # Namespace(foo="bar")
    # print(a.foo)
    # bar
    # print (b)
    # ["unknown argument 6969"]

    namespace, unknown = parser.parse_known_args()
    key = list(ord(y.lower()) % ord("a") for arg in namespace.k for y in arg)
    if not namespace.k.isalpha() or namespace.k==None:
        sys.exit(1)
    while True:
        try:
            plain = input("plain:")
        except ValueError or NameError:
            continue
        break

    cipher = ""
    k = sum(c.isalpha() for x in plain for c in x)
    l = len(plain)
    j = 0
    for i in range(l):
        if plain[i].isalpha():
            print(j)
            c = ord(plain[i]) + key[j % len(key)]
            if not chr(c).isalpha():
                if plain[i].isupper():
                    c = (c % (ord("Z") + 1)) + ord("A")
                else:
                    c = (c % (ord("z") + 1)) + ord("a")
            cipher += chr(c)
            j+=1

        else:
            cipher+= plain[i]





    print(key)
    print("plain:{}".format(plain))
    print("cipher:{}".format(cipher))







if __name__ == "__main__":
    main(sys.argv)