from sqlalchemy import *
from sqlalchemy.orm import *
from sqlalchemy.ext.declarative import *
from sqlalchemy import event
from sqlalchemy import inspect
from passlib.hash import bcrypt
from helpers import lookup



engine= create_engine("sqlite:///finance.db", case_sensitive=False, echo=False)
sess2 = sessionmaker(bind=engine)
_session = sess2()
q = lambda x: _session.query(x)


class Base(object):

    @declared_attr
    def __tablename__(cls):
        return (cls.__name__.lstrip("Table").lower() + "s")

    id = Column(Integer, primary_key=True)

    @declared_attr
    def name(cls):
        return Column(Text, nullable=False, server_default=cls.__name__.lower() + "s")

Base = declarative_base(cls=Base)


class User(Base):
    hash = Column(Text, nullable=True)
    cash = Column(Float(decimal_return_scale=2), nullable=False, server_default="10000")
    order = relationship("Order", backref="user")
    #port = relationship("Port", uselist=True, backref="user")
    def __verify__(self, pw):
        return (bcrypt.verify(pw, self.hash))


class Stock(Base):
    @declared_attr
    def symbol(cls):
        return Column(Text, nullable=True, server_default=cls.__tablename__.upper())
    price = Column(Float(decimal_return_scale=2), nullable=True)
    timestamp = Column(DateTime, server_default=func.current_timestamp())
    order = relationship("Order", backref="stock")
    #port = relationship("Port", backref="stock")

class Order(Base):
    user_id = Column(Integer, ForeignKey(User.id))
    stock_id = Column(Integer, ForeignKey(Stock.id))
    qty = Column(Integer, nullable=False, server_default="50")
    qty_remain = Column(Integer, nullable=True, server_default="50")
    snap = Column(Float(decimal_return_scale=2))
    action = Column(Boolean, nullable=False, server_default="1")
    active = Column(Boolean, nullable=False, server_default="1")
    timestamp = Column(DateTime, onupdate=func.current_timestamp())

    #port = relationship("Port", backref="order")


# insert <mapper event>
# session.add(Table)
@event.listens_for(User, "before_insert")
def _hash(mapper, engine, target):
    target.hash = bcrypt.hash(target.hash)

# update <mapper event>

@event.listens_for(User, 'before_update')
def _hash(mapper, connection, target):
    if inspect(target).attrs.hash.history.has_changes():
        target.hash = bcrypt.hash(target.hash)
        #print(target.hash)


@event.listens_for(Stock, 'before_update')
def _hash(mapper, connection, target):
    target.timestamp= func.current_timestamp()

@event.listens_for(Order, 'before_insert')
def _hash(mapper, connection, target):
    target.timestamp= func.current_timestamp()


_session.commit()

#Base.metadata.drop_all(engine)
Base.metadata.create_all(engine)
