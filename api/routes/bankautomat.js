const express = require('express');
const router = express.Router();
const bankautomat = require('../models/bankautomat_models');

router.post('/kirjaudu_sisaan', function(request, response){
	bankautomat.kirjaudu_sisaan(request.body, function(err, dbResult){
		if(err){
			response.json(err);
		}
		else{
			response.json(dbResult);
		}
	});
});

router.post('/nosta_rahaa', function(request, response){
	bankautomat.nosta_rahaa(request.body, function(err, dbResult){
		if(err){
			response.json(err);
		}
		else{
			response.json(dbResult);
		}
	});
});

router.post('/tarkista_saldo', function(request, response){
	bankautomat.tarkista_saldo(request.body, function(err, dbResult){
		if(err){
			response.json(err);
		}
		else{
			response.json(dbResult);
		}
	});
});

router.post('/selaa_tilitapahtumia', function(request, response){
	bankautomat.selaa_tilitapahtumia(request.body, function(err, dbResult){
		if(err){
			response.json(err);
		}
		else{
			response.json(dbResult);
		}
	});
});

module.exports = router;