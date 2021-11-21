const db = require('../database');

const accounts_has_customers={
    get: function(callback) {
        return db.query('select * from accounts_has_customers', callback);
    },
    getByAccId: function(id, callback) {
        return db.query('select * from accounts_has_customers where idAccount=?', [id], callback);
    },
    getByCustId: function(id, callback) {
        return db.query('select * from accounts_has_customers where idCustomer=?', [id], callback);
    },
    add: function(accounts_has_customers, callback) {
        return db.query('call lisaa_asiakas_tilille(?,?), [accounts_has_customers.idCustomer, accounts_has_customers.idAccount], callback);
    },
    delete: function(accounts_has_customers, callback) {
        return db.query('call poista_asiakas_tililta(?,?), [accounts_has_customers.idCustomer, accounts_has_customers.idAccount], callback);
    }
}

module.exports = accounts_has_customers;