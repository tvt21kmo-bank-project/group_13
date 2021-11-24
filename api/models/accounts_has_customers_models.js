const db = require('../database');

const accounts_has_customers = {

    lisaa_asiakas_tilille: function(postData, callback){
        return db.query('call lisaa_asiakas_tilille(?,?)',
        [postData.asiakasID, postData.tiliID],
        callback);
    },
    poista_asiakas_tililta: function(postData, callback){
        return db.query('call poista_asiakas_tililta(?,?)',
        [postData.asiakasID, postData.tiliID],
        callback);
    }
};

module.exports = accounts_has_customers;