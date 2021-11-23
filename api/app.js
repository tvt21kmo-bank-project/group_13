var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
var bankRouter = require('./routes/bankautomat');
var custRouter = require('./routes/customers');
var accRouter = require('./routes/accounts');
var cardRouter = require('./routes/cards');
var creditRouter = require('./routes/credit');
var transRouter = require('./routes/transactions');

var app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/users', usersRouter);
app.use('/bankautomat', bankRouter);
app.use('/customers', custRouter);
app.use('/accounts', accRouter);
app.use('/cards', cardRouter);
app.use('/credit', creditRouter);
app.use('/transactions', transRouter);

module.exports = app;
