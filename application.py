import os, decimal
import re
from flask import Flask, jsonify, json, render_template, request
from sqlalchemy import *
from sqlalchemy import event
from sqlalchemy.orm import *
from sqlalchemy.sql import *

from initbase import Tableplace
from cs50 import SQL
from helpers import lookup



# Configure application
app = Flask(__name__)
os.environ["API_KEY"] = "AIzaSyB_vXeX41RVScj-c3CwRjOoJFnGfyAkvmc"
engine = create_engine("sqlite:///initdb.db")
db = engine
metadata = MetaData(bind=engine)
table = Table("places", metadata, autoload=True, autoload_with=engine)


Sess = scoped_session(sessionmaker(bind=engine))
sess = null
_q = lambda x: sess.query(x)


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
def index():
    # using global variables
    # https://stackoverflow.com/a/10588651

    """Render map"""
    if not os.environ.get("API_KEY"):
        raise RuntimeError("API_KEY not set")
    return render_template("index.html", key=os.environ.get("API_KEY"))


@app.route("/articles")
def articles():
    # https://developers.google.com/loader/
    # google.load(api/module name, version, options)
    if request.method == "GET":
        q = request.args.get("q")
        results = lookup(q)
    # TODO
    return jsonify([results]);


@app.route("/search")
def search():
    global sess
    sess = Sess()
    """Search for places that match query"""


    if request.method == "GET":
        query = request.args.get("q")
        stmt = table.select()\
        .where(or_(\
                table.c.place_name.startswith(query) \
                ,table.c.admin_name1.startswith(query) \
                ,table.c.admin_name1.ilike(f"%{query}%")\
                , table.c.postal_code.startswith(query)\
                    )\
                )
        _r = _q(stmt).all()

        rows = []
        for row,i in zip(_r, range(len(_r))):
            rows.insert(i, dict())
            for k,u in zip(row.keys(), range(len(row.keys()))):
                rows[i].setdefault(k,str(row[u]))
    Sess.remove()
    return jsonify(rows)



@app.route("/update")
def update():
    """Find up to 10 places within view"""

    # Ensure parameters are present
    if not request.args.get("sw"):
        raise RuntimeError("missing sw")
    if not request.args.get("ne"):
        raise RuntimeError("missing ne")

    # Ensure parameters are in lat,lng format
    if not re.search("^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$", request.args.get("sw")):
        raise RuntimeError("invalid sw")
    if not re.search("^-?\d+(?:\.\d+)?,-?\d+(?:\.\d+)?$", request.args.get("ne")):
        raise RuntimeError("invalid ne")

    # Explode southwest corner into two variables
    sw_lat, sw_lng = map(float, request.args.get("sw").split(","))

    # Explode northeast corner into two variables
    ne_lat, ne_lng = map(float, request.args.get("ne").split(","))

    global sess
    sess = Sess()

    # Find 10 cities within view, pseudorandomly chosen if more within view
    if sw_lng <= ne_lng:

        # Doesn't cross the antimeridian
        _stmt = table.select().where(\
        and_(\
            table.c.latitude >= sw_lat,
            table.c.latitude <= ne_lat,
            and_(table.c.longitude >= sw_lng, table.c.longitude <= ne_lng))\
            )\
        .group_by(table.c.country_code, table.c.place_name, table.c.admin_code1)\
        .order_by(func.random())\
        .limit(10)
        result = _q(_stmt).all()


    else:

        # Crosses the antimeridian
        _stmt = table.select().where(\
        and_(\
            table.c.latitude >= sw_lat,
            table.c.latitude <= ne_lat,
            or_(table.c.longitude >= sw_lng, table.c.longitude <= ne_lng))\
            )\
        .group_by(table.c.country_code, table.c.place_name, table.c.admin_code1)\
        .order_by(func.random())\
        .limit(10)
        result = _q(_stmt).all()
    Sess.remove()
    rows = []
    for row,i in zip(result, range(len(result))):
        rows.insert(i, dict())
        for k,u in zip(row.keys(), range(len(row.keys()))):
            rows[i].setdefault(k,str(row[u]))

    # Output places as JSON
    return jsonify(rows)
