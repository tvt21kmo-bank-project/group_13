const db = require('../database');

const transactions={
    get: function(callback) {
        return db.query('select * from transactions', callback);
    },
    getById: function(id, callback) {
        return db.query('select * from transactions where idtransaction=?', [id], callback);
    },
    add: function(transactions, callback) {
        return db.query('insert into transactions (transaction_type,transaction_amount,transaction_time,idaccount,idcard) values (?,?,?,?,?)', 
		[transactions.transaction_type, transactions.transaction_amount, transactions.transaction_time, transactions.idaccount, transactions.idcard], callback);
    },
    delete: function(id, callback) {
        return db.query('delete from transactions where idtransaction=?', [id], callback);
    },
    update: function(id, transactions, callback) {
        return db.query('update transactions set credit_limit=?, balance=?, idaccount=? where idtransaction=?', 
		[transactions.transaction_type, transactions.transaction_amount, transactions.transaction_time, transactions.idaccount, transactions.idcard, id], callback);
    }
}

module.exports = transactions;