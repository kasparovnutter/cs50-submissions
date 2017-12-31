import csv
import urllib.request

def lookup(symbol):
    """Look up quote for symbol."""
    _symbol = symbol
    symbol = symbol.upper()
    # reject symbol if it starts with caret
    if symbol.startswith("^"):
        raise ValueError(f"\" {_symbol} \" is an invalid name.") from None

    # reject symbol if it contains comma
    if "," in symbol:
        raise ValueError(f"\" {_symbol} \" is an invalid name.") from None

    '''
    # query Yahoo for quote
    # http://stackoverflow.com/a/21351911
    try:

        # GET CSV
        url = f"http://download.finance.yahoo.com/d/quotes.csv?f=snl1&s={symbol}"
        webpage = urllib.request.urlopen(url)

        # read CSV
        datareader = csv.reader(webpage.read().decode("utf-8").splitlines())

        # parse first row
        row = next(datareader)

        # ensure stock exists
        try:
            price = float(row[2])
        except:
            raise ValueError(f"\"{symbol}\" is an invalid name.") from None

        # return stock's name (as a str), price (as a float), and (uppercased) symbol (as a str)
        return {
            "name": row[1],
            "price": price,
            "symbol": row[0].upper()
        }

    except:
        pass
    '''
    # query Alpha Vantage for quote instead
    # https://www.alphavantage.co/documentation/
    try:

        # GET CSV
        url = f"https://www.alphavantage.co/query?apikey=NAJXWIA8D6VN6A3K&datatype=csv&function=TIME_SERIES_INTRADAY&interval=1min&symbol={symbol}"
        webpage = urllib.request.urlopen(url)

        # parse CSV
        datareader = csv.reader(webpage.read().decode("utf-8").splitlines())

        # ignore first row
        next(datareader)

        # parse second row
        row = next(datareader)

        # ensure stock exists
        try:
            price = float(row[4])
        except:
            raise ValueError(f"\" {_symbol} \" is an invalid name.") from None

        # return stock's name (as a str), price (as a float), and (uppercased) symbol (as a str)
        return {
            "name": symbol.upper(), # for backward compatibility with Yahoo
            "price": price,
            "symbol": symbol.upper()
        }

    except:
        raise ValueError(f"\" {_symbol} \" is an invalid name.") from None


def usd(value):
    """Formats value as USD."""
    return f"${value:,.2f}"
