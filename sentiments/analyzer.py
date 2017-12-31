from nltk.tokenize import TweetTokenizer
import os, sys


class Analyzer:

    """Implements sentiment analysis."""

    def __init__(self, **kwargs):
        self.twtoken = TweetTokenizer(**kwargs)
        self.pospath = os.path.join(sys.path[0], "positive-words.txt")
        self.negpath = os.path.join(sys.path[0], "negative-words.txt")
        self.bankpath = [self.pospath, self.negpath]
        #print(sys.path[0])
        self.bank = {}
        for path in self.bankpath:
            name = (os.path.splitext(os.path.split(path)[1]))[0].partition("-")[0]

            nametext = set()
            with open(path, "rb") as file:
                while True:
                    initpos = file.tell()
                    _char = file.read(sys.getsizeof("a"))
                    if _char == ";":
                        file.readline()
                        continue
                    elif not _char:
                        break
                    else:
                        offset = file.tell() - initpos
                        file.seek(-offset, 1)
                        word  = file.readline().strip()
                        nametext.add(word)

            self.bank[name] = nametext


        # io module - https://docs.python.org/3/library/io.html
        # ===========
        # open() = returns <file object of I/O>
        # (file, mode='r', buffering=-1, encoding=None, errors=None,
        # newline=None, closefd=True, opener=None)

        # (mode = "b"),                 returns <Binary I/O file object>
        # (mode = "b", buffering = 0),  returns <Raw I/O file object>

        # default                       returns <Binary I/O file object>
        #create a dict for variable names

        # os.path.split(path) = splits a <path object> or str
        #       C:\folder1\folder2\file.txt to
        #       ("C:\folder1\folder2\", "file.txt")

        # os.path.splitext(pathname) = splits a str
        #       "file.txt" to
        #       ("file", ".txt")

        #self.ref is a dictionary
        # {x: set (self.paths[0]), y : set (self.paths[1])}



    def analyze(self, text="I love you", **kwargs):
        """Analyze text for sentiment, returning its score."""
        # tokenize text (parse sentences)
        if isinstance(text, str):
            text = {"text": text}
        tokentext = self.twtoken.tokenize(text["text"])
        text["text"] = dict.fromkeys(text["text"].split(),)
        text["score"] = 0
        for word, i in zip(tokentext,range(len(tokentext))):
            if word.isalpha():
                #wpos = text["text"].find(word)
                wordb = word.encode()
                if wordb in self.bank["positive"]:
                    text["text"][word] = 1
                elif wordb in self.bank["negative"]:
                    text["text"][word] = -1
        for x in iter(text["text"]):
            if text["text"].get(x):
                text["score"] += text["text"].get(x)
        return text

