const db = require('../database');

const customers={
    get: function(callback) {
        return db.query('select * from customers', callback);
    },
    getById: function(id, callback) {
        return db.query('select * from customers where idcustomer=?', [id], callback);
    },
    add: function(customers, callback) {
        return db.query('call lisaa_asiakas (?, ?, ?)', 
			[ customers.etunimi, customers.sukunimi, customers.tiliID], callback);
    },
    delete: function(id, callback) {
        return db.query('call poista_asiakas(?)', [id], callback);
    },
    update: function(id, customers, callback) {
        return db.query('update customers set firstname=?,lastname=? where idcustomer=?', [ customers.firstname, customers.lastname, id], callback);
    }
}

module.exports = customers;