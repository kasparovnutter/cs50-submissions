from flask import *
from functools import wraps

from base import *
from sqlalchemy import inspect
from sqlalchemy.orm import attributes as a
from sqlalchemy import exc
import sqlalchemy.orm.exc as ormexc
from helpers import lookup
from wtforms import Form, BooleanField, StringField, PasswordField, validators
import logging
import time



logging.basicConfig()
logging.getLogger("sqlalchemy.dialect").setLevel(20)

app = Flask(__name__)
app.jinja_env.line_statement_prefix = '#'
app.secret_key = '7698889887'

# session = by default, Flask's session object (if running)
fsess = session
dbsess = sess2()
q = lambda x: dbsess.query(x)

redir = lambda x: redirect(url_for(x))
rform = lambda x:request.form.get(x)
def response():
    return (make_response())

class Loginform(Form):
    username = StringField('username', [validators.Length(min=4, max=25)])
    password = PasswordField('New Password', [
        validators.DataRequired(),
        validators.EqualTo('confirm', message='Passwords must match')
    ])
    confirm = PasswordField('Repeat Password', [validators.DataRequired()])

class NotEqualTo(object):
    def __init__(self, fieldname, message=None):
        self.fieldname = fieldname
        self.message = message

    def __call__(self, form, field):
        try:
            other = form[self.fieldname]
        except KeyError:
            raise validators.ValidationError("{} is an invalid field".format(self.fieldname))
        if other.data == field.data:
            message = self.message
            if message is None:
                message = field.gettext("Passwords must be different")
            raise validators.ValidationError(message)


class PwForm(Form):
    # Custom Validators | WTForms
    # https://goo.gl/atMRHG
    oldpassword = PasswordField('Old Password',
    [validators.DataRequired()]
    )
    newpassword = PasswordField('New Password',
    [validators.DataRequired(), NotEqualTo("oldpassword")]
    )



def login_required(func):
    @wraps(func)
    def new_func(*args, **kwargs):
        # assert fsess.get("account"), "init a user first"
        # stmt = check if database has users
        stmt = q(q(User).exists()).scalar()
        if not fsess.get("account") or not stmt:
            print("Not logged in")
            return redir("login")
        else:
            try:
                fsess["account"]["cash"] = q(User).get(fsess.get("account").get("id")).cash
            except:
                pass
        return func(*args, **kwargs)
    return new_func

@app.route("/apology")
def apology():
    refer = request.args.get("next") or request.referrer or url_for("index")
    return render_template("apology.html", referlink=refer)



@app.route("/", methods=["GET", "POST"])
@app.route("/index")
@login_required
def index():
    #_portfull
    #test_login()
    try:
        _quser = q(User).get(fsess.get("account").get("id"))
        _qport = q(Order).filter(Order.user_id==_quser.id, Order.active==True)
    except Exception as e:
        flash(e, "err")
        return redir("login")


    # _portfull = { stock : total qty }
    _portfull = dict()
    _porttotal = 0


    stmt = q(Order).filter(Order.user_id==_quser.id, Order.active==True).exists()

    if q(stmt).scalar()==True:
        _portfull["stocks"] = dict()
        #get (distinct) stock names that are active
        _uniqname = set()

        try:
            _port = _qport.all()
            for port in _port:
                _uniqname.add(port.stock.name)
        except Exception as e:
            flash(e, "err")
            return redirect(url_for("apology"))
            #return response()



        for name in _uniqname:
            _portfull["stocks"][name] = dict()
            _id = q(Stock).filter(Stock.name==name).first().id
            _stockactive = q(Order).filter(Order.stock_id==_id, Order.user_id==_quser.id,Order.active==True).all()
            _sum = 0
            for stock in _stockactive:
                _sum += stock.qty_remain
            _portfull["stocks"][name].setdefault("name", name)
            _portfull["stocks"][name].setdefault("qty", _sum)

        for item in _portfull["stocks"].keys():
            try:
                _result= lookup(item)
                _price = _result.get("price")
                _val = _portfull.get("stocks").get(item).get("qty") * _result.get("price")
                _portfull["stocks"][item].setdefault("price", _price)
                _portfull["stocks"][item].setdefault("total", format(_val, ".2f"))
                _porttotal += _val
            except:
                pass
    return render_template("index.html", port=_portfull, total=format(_porttotal,".2f"))



@app.route("/register", methods=["POST", "GET"])
def register():
    form = Loginform(request.form)
    if request.method=="POST":
        if not request.form.get('username') or not request.form.get('password'):
            return redir("apology")
        if form.validate():
            newuser=dict(name=rform("username"), hash=rform("password"))
            _query = q(User).filter(User.name==newuser.get("name"))
            if _query.first():
                flash("Username taken", "err")
                return redir("apology")
                #return response()
            else:
                dbsess.add(User(**newuser))
                dbsess.commit()
                flash("Registered", "register")

        else:
            if request.form.get("confirm"):
                flash("passwords do not match", "err")
            else:
                flash("password must be inputted twice", "err")
            return redir("apology")
            #return response()

    return render_template("register.html", form=form)


@app.route("/login", methods=["GET", "POST"])
def login():

    if request.method=="POST" and request.form:
        newuser = dict(name=rform("username"), hash=rform("password"))
        _query = q(User).filter(User.name==newuser.get("name"))
        try:
            _q = _query.one()
            #print(_q.__verify__(newuser.get("hash")))
            if _q.__verify__(newuser.get("hash")):
                if fsess.get("account") == newuser.get("name"):
                    flash("user is already logged in", "err")
                #if fsess.get("account"):
                    # popup dialog box ("User is already logged in. Confirm logout?")
                #if request.form.get["rmb"]:
                    #for attr in inspect(_q).attrs.keys():
                        #fsess["account"][attr] = _q.attr
                fsess["account"] = {}
                for attr in _q.__dict__.items():
                    try:
                        if attr[0].startswith("_"):
                            continue
                        fsess["account"].setdefault(attr[0], attr[1])
                        fsess.modified=True
                    except AttributeError as e:
                        pass


                flash("{}".format(_q.name), "login")
                return redir("index")
            else:
                flash("Invalid credentials.", "err")
        except:
            flash("Invalid user.", "err")
            #return redirect(url_for("apology"))
            #return response()

    return render_template("login.html")

@app.route("/logout")
def logout():
    flash("Logged out {}".format(fsess.get("account").get("name")), "err")
    fsess.pop("account",None)
    #print("logout :{}".format(fsess.get("account")))
    return redirect(url_for("login"))

@app.route("/newpass", methods=["GET", "POST"])
@login_required
def password():
    form = PwForm(request.form)
    if request.method=="POST" and request.form:
        _quser = q(User).get(fsess.get("account").get("id"))
        if _quser.__verify__(request.form.get("oldpassword")):
            if form.validate():
                a.set_attribute(_quser, "hash", request.form.get("newpassword"))
                a.flag_modified(_quser, "hash")
                dbsess.commit()
                fsess["account"]["password"] = _quser.hash
                fsess.modified=True
                flash("password changed.", "password")
            else:
                for field in form.errors.keys():
                    flash(form.errors.get(field), "err")

        else:
            flash("Incorrect old password", "err")
            #return response()
            #return redirect(url_for("apology"))
    return render_template("password.html", form=form)


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    if request.method=="GET" and request.args:
        ticker = request.args.get("ticker").upper()
        try:
            _result = lookup(ticker)
            _q = q(Stock).filter(Stock.name==ticker)
            try:
                x = _q.one()
                for attr in _result:
                    setattr(x, attr, _result.get(attr))
            except Exception as e:
                dbsess.add(Stock(**_result))
                dbsess.commit()
            #print(_result)
            return render_template("quoted.html", result=_result)
        except Exception as e:
            flash(str(e), "err")
            #return response()
    return (render_template("quote.html"))



@app.route("/buy", methods=["POST", "GET"])
@login_required
def buy():
    result=None
    if request.method == "GET" and request.args:
        ticker = request.args.get("symbol")
        amt = request.args.get("qty")
        if amt==None or amt=="":
            amt= 0
        try:
            amt = int(amt)
        except ValueError as e:
            flash("Invalid input(qty:{} is invalid)".format(amt), "err")
            return redirect(url_for("apology"))
            #return response()

        if amt <=0 or ticker=="":
            if not ticker=="":
                flash("Invalid input(qty:{} is invalid)".format(amt), "err")
            elif amt > 0:
                flash("Input is blank".format(ticker), "err")
            else:
                flash("Invalid input({}, qty:{} is invalid)".format(ticker, amt), "err")
            return redirect(url_for("apology"))
            #return response()
        else:
            try:

                _quser = q(User).get(fsess.get("account").get("id"))
                _qstock = q(Stock).filter(Stock.name==ticker.upper())
                _qorder = lambda: q(Order).filter(Order.stock_id==_qstock.one().id, Order.user_id==_quser.id)
                _result = lookup(ticker)
                _val = _result.get("price") * amt
                if (_quser.cash - _val) < 0.00:
                    flash("Insufficient funds. (cost: ${:.2f})".format(_val), "err")
                    _result.clear()
                    return redirect(url_for("apology"))
                    #return response()
                else:
                    try:
                        _stk = _qstock.one()
                        a.set_attribute(_stk, 'price', _result.get("price"))
                    except ormexc.MultipleResultsFound:
                        pass
                    except ormexc.NoResultFound:
                        dbsess.add(Stock(**_result))
                        dbsess.commit()

                    stmt = Order(\
                            user_id=_quser.id\
                            ,stock_id=_qstock.one().id\
                            ,qty=amt\
                            ,qty_remain=amt\
                            ,action=True\
                            ,snap=_result.get("price")\
                            ,timestamp=_qstock.one().timestamp\
                            ,active=True
                    )
                    #print(_qstock.one().timestamp)
                    dbsess.add(stmt)
                    dbsess.commit()

                    _newcash = _quser.cash - _val
                    a.set_attribute(_quser, 'cash', _newcash)
                    '''
                    for item in _qorder().filter(Order.active==True).all():
                        print(item)
                    '''
                    flash("success", "buy")
                    fsess["account"]["cash"] = _quser.cash
                    fsess.modified = True
                    _order = _qorder().order_by(Order.timestamp.desc()).first().__dict__
                    _order.pop("_sa_instance_state", None)
                    result=_order

            except Exception as e:
                flash(f"{e}", "err")
                return redirect(url_for("apology"))
                #return response()
    #print("buy :{}".format(fsess.get("account")))
    return render_template("buy.html", result=result)



@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    #_sum is wrong, todo
    #_sum, _stockactive
    _order=None
    if request.method=="POST" and request.form:
        _quser = q(User).get(fsess.get("account").get("id"))
        _qstock = lambda: q(Stock).filter(Stock.name==ticker).one()
        _qorder = lambda: q(Order).filter(Order.stock_id==_qstock().id, Order.user_id==_quser.id,Order.active==True).all()
        _result = lambda: lookup(ticker)
        ticker = rform("symbol").upper().strip()
        try:
            x = _qorder()
            if not x:
                raise Exception
        except Exception as e:
            flash("No {} stocks owned".format(ticker), "err")
            return redirect(url_for("apology"))
            #return response()

        _amt = rform("qty")
        _sum = 0
        _owned = 0
        if _amt==None or _amt=="":
            _amt= 0

        try:
            _amt = int(_amt)
            if _amt <=0 or ticker=="":
                if not ticker=="":
                    flash("Invalid input(qty:{} is invalid)".format(_amt), "err")
                elif _amt > 0:
                    flash("Invalid input({} is invalid)".format(ticker), "err")
                else:
                    flash("Invalid input({}, qty:{} is invalid)".format(ticker, _amt), "err")
        except ValueError as e:
            flash("Invalid input(qty:{} is invalid)".format(_amt), "err")
            return redirect(url_for("apology"))
            #return response()


        _amtcount = _amt
        try:
            _qstock()
        except Exception as e:
            flash(e,"err")
            return redirect(url_for("apology"))
            #return response()
        _stockactive = q(Order)\
            .filter(Order.user_id == _quser.id, Order.stock_id==_qstock().id, Order.active==True)\
            .order_by(Order.timestamp.asc())\
            .all()
        for instance in _stockactive:
            _owned += instance.qty_remain
        if _owned < _amt:
            flash("Insufficient stocks (Owned: {}, Requested: {})".format(_owned, _amt), "err")
            return redirect(url_for("apology"))
            #return response()
        else:
            try:
                _result()
            except Exception as e:
                flash(f"{e}", "err")
                return redirect(url_for("apology"))
                #return response()
            _result = _result()
            for instance in _stockactive:
                #print("id:{} remain:{} amt:{} total:{}".format(instance.id, instance.qty_remain, _amt, _sum))
                if _amt == 0:
                    break
                if instance.qty_remain > _amt:
                    _remain = instance.qty_remain - _amt
                    #print(_remain)
                    _amt = 0
                    a.set_attribute(instance, "qty_remain", _remain)
                    dbsess.commit()
                elif instance.qty_remain <= _amt:
                    # TODO pass _amt as a signed integer (unsigned currently)
                    _amt -= instance.qty_remain
                    a.set_attribute(instance, "active", False)
                    a.set_attribute(instance, "qty_remain", 0)
                    dbsess.commit()
                _sum += (instance.qty_remain * instance.snap)

                #print(_sum)


            stmt = Order\
            (user_id=_quser.id, stock_id=_qstock().id\
            ,snap=_result.get("price"),qty=_amtcount\
            ,active=False, action=False)
            dbsess.add(stmt); dbsess.commit()

            _newcash = _quser.cash + (_result.get("price") * _amtcount)
            _net = (_result.get("price") * _amtcount) - _sum
            a.set_attribute(_quser, "cash", _newcash)
            fsess["account"]["cash"] = _quser.cash
            fsess.modified= True
            flash("success", "sell")
            _order = q(Order).filter(Order.user_id==_quser.id, Order.stock_id==_qstock().id, Order.active==False)\
                    .order_by(Order.timestamp.desc()).first().__dict__
            _order.pop("_sa_instance_state", None)
            '''
            _stockactive= q(Order)\
                .filter(Order.user_id == _quser.id, Order.stock_id==_qstock().id, Order.active==True)\
                .order_by(Order.timestamp.desc())\
                .all()
            for order in _stockactive:
                print(order.id, order.timestamp, order.qty_remain)
            '''
    #print("sell :{}".format(fsess.get("account")))
    return render_template("sell.html", results=_order)


@app.route("/history")
@login_required
def history():
    history = dict()
    try:
        rows = q(Order).filter(Order.user_id==fsess.get("account").get("id")).order_by(Order.timestamp.desc())\
        .all()
    except Exception as e:
        rows = None
    if rows:
        for row, i in zip(rows, range(len(rows))):
            history[i] = dict()
            history[i].setdefault("action", ("Buy" if row.action else "Sell"))
            history[i].setdefault("qty", row.qty)
            history[i].setdefault("symbol", row.stock.symbol)
            history[i].setdefault("snap", row.snap)
            history[i].setdefault("timestamp", row.timestamp)

    return render_template("history.html", order=history)
        #history["row"].setdefault()






