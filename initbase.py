from sqlalchemy import Column, Integer, String, Text, Table
from sqlalchemy.types import *
from sqlalchemy import create_engine, event, inspect, MetaData
from sqlalchemy.orm import sessionmaker, create_session, mapper
from sqlalchemy.ext.declarative import declarative_base, declared_attr

import json

engine = create_engine("sqlite:///initdb.db", case_sensitive=False, echo=True)
metadata = MetaData(bind=engine)



class Tableplace(object):
    pass

# https://goo.gl/h7DLUe (column_reflect event)
# https://goo.gl/ootTz5 (column_reflect listen e.g)
# https://goo.gl/7MS5ra (column .key attribute)

def _event(inspector, table, column_info):
    # column_info["key"] = column identifier in  python
    # column_info["name"] = column identifier in SQL
    column_info["key"] = column_info["name"].replace(" ", "_")


event.listen(Table, "column_reflect", _event)


def main():

    # using Table object and mapping to Class w mapper (https://goo.gl/JZPKwN)
    place = Table("places", metadata\
                ,Column("id", Integer, primary_key=True)\
                # x = <str type>, y = <str type> , eval(y) = <"y" type>
                ,*(Column(x,eval(y), nullable=True, primary_key=False) for (x,y) in (field for field in fields.items()))\
                , keep_existing=True\
                , listeners=[("column_reflect", _event)]\
                )


    metadata.create_all()
    mapper(Tableplace, place)
    _sess = create_session(bind=engine, autocommit=True)

    with open("initplace.txt", "r") as f:
        for place in json.load(f):
            _sess.begin()
            u = Tableplace()
            for field in place.keys():
                setattr(u, field, place.get(field))
                _sess.add(u)
            _sess.commit()
    _sess.close()


if __name__ == "__main__":
    fields = json.load(open("initfield.txt", "r"))
    _q = lambda x: _sess.query(x)
    main()