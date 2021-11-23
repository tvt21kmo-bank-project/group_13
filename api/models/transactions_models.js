const db = require('../database');

const transactions={
    get: function(callback) {
        return db.query('select * from transactions', callback);
    },
    getById: function(id, callback) {
        return db.query('select * from transactions where idtransaction=?', [id], callback);
    },
    add: function(transactions, callback) {
        return db.query('insert into transactions (transaction_type,transaction_amount,idaccount,idcard) values (?,?,?,?)', 
		[transactions.transaction_type, transactions.transaction_amount, transactions.idaccount, transactions.idcard], callback);
    },
    delete: function(id, callback) {
        return db.query('delete from transactions where idtransaction=?', [id], callback);
    },
    update: function(id, transactions, callback) {
        return db.query('update transactions set transaction_type=?, transaction_amount=?, idaccount=?, idcard=? where idtransaction=?', 
		[transactions.transaction_type, transactions.transaction_amount, transactions.idaccount, transactions.idcard, id], callback);
    }
}

module.exports = transactions;