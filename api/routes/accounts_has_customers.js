const express = require('express');
const router = express.Router();
const accounts_has_customers = require('../models/accounts_has_customers_models');

router.post('/lisaa_asiakas_tilille', function(request, response){
	accounts_has_customers.lisaa_asiakas_tilille(request.body, function(err, dbResult){
		if(err){
			response.json(err);
		}
		else{
			response.json(dbResult);
		}
	});
});

router.post('/poista_asiakas_tililta', function(request, response){
	accounts_has_customers.poista_asiakas_tililta(request.body, function(err, dbResult){
		if(err){
			response.json(err);
		}
		else{
			response.json(dbResult);
		}
	});
});

module.exports = router;