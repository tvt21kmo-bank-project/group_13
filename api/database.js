const mysql = require('mysql');
const connection = mysql.createPool({
  host: 'localhost',
  user: 'bank_simul_user',
  password: '1234',
  database: 'bank_simul'
});
module.exports = connection;