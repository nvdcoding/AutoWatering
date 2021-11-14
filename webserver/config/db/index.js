
const mongoose = require('mongoose');

module.exports = {
    connect: async () => {
        try {
            await mongoose.connect('mongodb://localhost:27017/cnpmn', {
                useNewUrlParser: true,
                useUnifiedTopology: true
            });
            console.log("success");
        } catch (error) {
            console.log(error);
        }
    }
}