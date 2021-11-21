const express = require('express');
const router = express.Router();
const customers = require('../models/customers_model');

router.get('/:id?',
function(request, response) {
    if (request.params.id) {
        customers.getById(request.params.id, function(err, dbResult) {
            if (err) {
                response.json(err);
            } else {
                response.json(dbResult);
            }
        });
    } else {
        customers.get(function(err, dbResult) {
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
    customers.add(request.body, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(request.body);
        }
    });
});

router.delete('/:id',
function(request, response) {
    customers.delete(request.params.id, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

router.put('/:id',
function (request, response) {
    console.log(request.body)
    customers.update(request.params.id, request.body, function(err, dbResult) {
        if (err) {
            response.json(err);
        } else {
            response.json(dbResult);
        }
    });
});

module.exports = router;