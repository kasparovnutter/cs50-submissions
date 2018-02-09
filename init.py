import json

def main():
    fields = {}
    with open("readme.txt", "r") as f:
        _flag = 0
        # x.splitlines() = ['text here']
        # x.splitlines()[0] = 'text here'
        for line in (x.splitlines()[0] for x in f):
            if _flag == 0:
                if "==" in line:
                    _flag = 1
                else:
                    pass
            # ignore empty lines (e.g line == "" )
            elif line:
                _l = line.split(":")
                # .strip() to remove whitespace
                fields[_l[0].strip()] = _l[1].strip().upper()

    places = []
    with open("US.txt", "r") as f:
        _fieldname = list(fields.keys())
        _fieldcount = len(_fieldname)
        # reset cursor
        _linecount = sum(1 for i in f)
        f.seek(0)

        for (line, i) in zip( (x.splitlines()[0] for x in f), range(_linecount) ):
            places.append({})
            for item, k in zip(line.split("\t"), range(_fieldcount)):
                if item:
                    places[i].setdefault(_fieldname[k], item.strip())
    with open("initfield.txt", "w") as f:
        f.write(json.dumps(fields))
    with open("initplace.txt", "w") as f:
        f.write(json.dumps(places))









if __name__ == "__main__":
    main()