#!/usr/bin/env python3
import os, sys, inspect
from flask import Flask, render_template, request, json, url_for
from analyzer import Analyzer
from tweets import Twit
import plotly.offline as py
import plotly.graph_objs as go
import colorlover as cl
from IPython.display import HTML
twitter = Twit()
what = Analyzer()
app = Flask(__name__.split(".")[0])



@app.route("/")
def index():
    return render_template("index.html")

@app.route("/search/")
def search():
    #http://flask.pocoo.org/docs/0.12/api/#incoming-request-data
    username = request.args.get("screen_name", type=None)
    if not username:
        username = inspect.signature(twitter.usertimeline).parameters["screen_name"].default.lstrip("@")
    try:
        timeline = twitter.usertimeline(username, count=100)
        user = twitter.user(username)
    except Exception as e:
        return render_template("search.html", plot="{}".format(e))

    x = y = z = 0
    for tweet in timeline:
        what.analyze(tweet)
        if tweet["score"] >= 1:
            x += 1
        elif tweet["score"] < 0:
            y += 1
        else:
            z += 1
    labels = ["positive", "negative", "neutral"]
    colors = cl.scales["8"]["qual"]["Pastel2"]
    trace = go.Pie(
            labels=labels, values=[x,y,z],
            marker=dict(
                    colors=[colors[0], colors[3], colors[5]]
                    #,line=dict(color=colors[4], width=4)
                    ),
            #hoverinfo="percent+value",
            hoverlabel=dict(
                bgcolor=colors[4],
                font=dict(
                    color="pastel blue")
                    )
            ,textfont=dict(
                family="Droid Sans",
                size=16,
                color=colors[7])
            ,textposition="inside"
            ,showlegend= False
            )
    layout = go.Layout(
            )
    plot = py.plot([trace], output_type="div")

    return render_template("search.html", screen_name=username, user=user, timeline=timeline,
    color= HTML(cl.to_html( cl.scales["8"]["qual"]["Pastel2"] )), plot=plot)

