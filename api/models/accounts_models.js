const db = require('../database');

const accounts={
    get: function(callback) {
        return db.query('select * from accounts', callback);
    },
    getById: function(id, callback) {
        return db.query('select * from accounts where idaccount=?', [id], callback);
    },
    add: function(accounts, callback) {
        return db.query('call lisaa_tili (?)', 
			[ accounts.asiakasID], callback);
    },
    delete: function(id, callback) {
        return db.query('call poista_tili(?)', [id], callback);
    },
    update: function(id, accounts, callback) {
        return db.query('update accounts set idOwner=?, balance=? where idaccount=?', [accounts.idOwner, accounts.balance, id], callback);
    }
}

module.exports = accounts;