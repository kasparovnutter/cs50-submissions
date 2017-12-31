import inspect

def ctype(a):
    #a is still a string
    #b is a string, elements a[0] and a[1] combined

    b = "".join(map(str, a[:2]))
    if len(a) == 15 and (int(b) == 34 or 37):
        return "AMEX"
    elif len(a) == 16 and (55 >= int(b) >=51):
        return "MASTERCARD"
    elif len(a) == 13 or 15 and int(b[0]) == 4:
        return "VISA"
    else:
        return None


def Luhns(a):
    t = 0

    a = tuple(map(int, str(a) ) )
    #multiply second-to-last digit
    b =  [\
        sum(int(y) for y in str(x)) \
        for x in (x*2 for x in a[-2:: -2])\
        ]
    #list = ( x*2, for x in a[-2:: -2] )
    #list = [ sum( int(digit), for digit in str(x) ), for x in list ]


    t = sum(b) + sum(x for x in a[-1:: -2])
    '''
    print\
    ("[{}]:{} ({})".format\
    #inspect.stack()
    #A list of named tuples
    #FrameInfo(frame, filename, lineno, function, code_context, index)
    #is returned. [0] = this stack, [3] = frameinfo(function)
    (inspect.stack()[0][3],t, "PASS" if (t%10==0) else "FAIL")\
    )
    '''

    return True if (t % 10 == 0) else False

def main():

    while True:
        try:
            cnum= input("input:")
            #if input is positive
            if (int(cnum) <= 0):
                continue
        except NameError and ValueError:
            continue
        #if input's actually a number
        except not cnum.isdigit:
            continue
        break
    if (ctype(cnum) != None):
        if(Luhns(cnum)):
            print(ctype(cnum))
        else:
            print("INVALID")
    else:
        print("INVALID")
        #print("huh {}".format(len(cnum)))
    return 0



if __name__ == "__main__":
    main()
