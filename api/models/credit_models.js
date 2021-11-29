const db = require('../database');

const credit={
    get: function(callback) {
        return db.query('select * from credit', callback);
    },
    getById: function(id, callback) {
        return db.query('select * from credit where idcredit=?', [id], callback);
    },
    add: function(credit, callback) {
        return db.query('insert into credit (credit_limit,balance,idaccount) values (?,?,?)', 
		[credit.credit_limit, credit.balance, credit.idaccount], callback);
    },
    delete: function(id, callback) {
        return db.query('delete from credit where idcredit=?', [id], callback);
    },
    muokkaa_luottoa: function(credit, callback) {
        return db.query('call muokkaa_luottoa(?,?,?)', 
		[credit.credit_limit, credit.balance, credit.idaccount], callback);
    }
}

module.exports = credit;