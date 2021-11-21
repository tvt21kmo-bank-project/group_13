 const db = require('../database');

const customers={
    get: function(callback) {
        return db.query('select * from customers', callback);
    },
    getById: function(id, callback) {
        return db.query('select * from customers where idcustomer=?', [id], callback);
    },
    add: function(customer, callback) {
        return db.query('insert into customers (firstname,lastname) values (?,?)', [ customers.firstname, customers.lastname, ], callback);
    },
    delete: function(id, callback) {
        return db.query('delete from customers where idcustomer=?', [id], callback);
    },
    update: function(id, customers, callback) {
        return db.query('update customers set firstname=?,lastname=? where idcustomer=?', [ id, customers.firstname, customers.lastname], callback);
    }
}

module.exports = customers;