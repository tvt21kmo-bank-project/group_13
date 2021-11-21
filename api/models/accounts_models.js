const db = require('../database');

const accounts={
    get: function(callback) {
        return db.query('select * from accounts', callback);
    },
    getById: function(id, callback) {
        return db.query('select * from accounts where idaccount=?', [id], callback);
    },
    add: function(accounts, callback) {
        return db.query('insert into accounts (idOwner,balance) values (?,?)', [accounts.idOwner, accounts.balance], callback);
    },
    delete: function(id, callback) {
        return db.query('delete from accounts where idaccount=?', [id], callback);
    },
    update: function(id, accounts, callback) {
        return db.query('update accounts set idOwner=?, balance=? where idaccount=?', [accounts.idOwner, accounts.balance, id], callback);
    }
}

module.exports = accounts;