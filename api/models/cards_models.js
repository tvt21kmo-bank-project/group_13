const db = require('../database');

const cards={
	get: function(callback) {
		return db.query('select * from cards', callback);
	},
	getById: function(id, callback) {
		return db.query('select * from cards where idcard=?', [id], callback);
	},
	add: function(cards, callback) {
		return db.query('insert into cards (pin, failed_attempts, has_credit, idaccount, idcustomer) values (?,?,?,?,?)',
		[cards.pin, cards.failed_attempts, cards.has_credit, cards.idaccount, cards.idcustomer], callback);
	},
		delete: function(id, callback) {
		return db.query('delete from cards where idcard=?', [id], callback);
	},
	update: function(id, cards, callback) {
		return db.query('update cards set pin=?, failed_attempts=?, has_credit=?, idaccount=?, idcustomer=? where idcard=?',
		[cards.pin, cards.failed_attempts, cards.has_credit, cards.idaccount, cards.idcustomer, id], callback);
	}
}

module.exports = cards;