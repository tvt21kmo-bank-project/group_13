const express = require('express');
const router = express.Router();
const credit = require('../models/credit_models');

router.get('/:id?',
function(request, response) {
    if (request.params.id) {
        credit.getById(request.params.id, function(err, dbResult) {
            if (err) {
                response.json(err);
            } else {
                response.json(dbResult);
            }
        });
    } else {
        credit.get(function(err, dbResult) {
            if (err) {
                response.json(err);
            } else {
                response.json(dbResult);
            }
        });
    }
});

router.post('/',
function(request, response) {
    credit.add(request.body, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(request.body);
        }
    });
});

router.delete('/:id',
function(request, response) {
    credit.delete(request.params.id, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

router.post('/muokkaa_luottoa',
function (request, response) {
    credit.muokkaa_luottoa(request.body, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

module.exports = router;