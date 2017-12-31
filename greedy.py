quarters = 0.25
dimes = 0.10
nickels = 0.05
pennies = 0.01
ch2 = (quarters,dimes,nickels,pennies);
total = 0

while True:
    try:
        ch = float( input("change owed:") )
    except NameError and ValueError:
        continue

    # try/except is only used for errors with SINGLE value
    # (ie range doesn't work)
    if (ch <= 0):
        continue
    break

for i in range( len(ch2) ):
    while not (round(ch,2) - ch2[i] < 0.00):
        ch -= ch2[i]
        total+=1
        #print("{:.2f} | {:.2f} | {}".format(ch, ch2[i], total))
print(total)


