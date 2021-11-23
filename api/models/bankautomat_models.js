const db = require('../database');

const bankautomat = {

    kirjaudu_sisaan: function(postData, callback){
        return db.query('call kirjaudu_sisaan(?,?)',
        [postData.korttiID, postData.korttiPIN],
        callback);
    },
    nosta_rahaa: function(postData, callback){
        return db.query('call nosta_rahaa(?,?,?,?)',
        [postData.tiliID, postData.korttiID, postData.luotto,postData.summa],
        callback);
    },
	tarkista_saldo: function(postData, callback){
        return db.query('call tarkista_saldo(?,?)',
        [postData.tiliID, postData.luotto],
        callback);
	},
	selaa_tilitapahtumia: function(postData, callback){
        return db.query('call selaa_tilitapahtumia(?,?)',
        [postData.tiliID, postData.alkaen_tapahtumasta],
        callback);
	}
};

module.exports = bankautomat;