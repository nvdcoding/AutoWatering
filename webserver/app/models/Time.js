const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const Time = new Schema({
    stamp: {type: String},
    sum: {type: Number}
});
module.exports = mongoose.model('Time', Time);