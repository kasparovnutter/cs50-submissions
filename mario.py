while True:
    try:
        h = int(input("Height:"))
    except ValueError:
        continue
    if not 23 >= h > 0:
        continue
    break

gap = 2
py  = ""
height = h+1

for row in range(1,height):
    py +=   (   "#" * row       \
                + " " * gap     \
                + "#" * row     \
            ).center(height * 2)
    py += "\n"

print(py)
