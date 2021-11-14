
const express = require('express');
const app = express();
const path = require('path');

const port = 3000;
const route = require('./routes');
const { client , channel} = require('./client');
const db = require('./config/db');

app.use(express.static('resources/public'))
app.use(express.urlencoded({extended: true}));
app.use(express.json())
client.subscribe(channel('update'));

db.connect();

client.on('message', (topic, message) =>{

});
  
app.get('/', (req, res) => {

    res.sendFile(path.resolve(__dirname, 'resources', 'views', 'main.html'));
});

route.run(app);

app.listen(port, () => {
    console.log(`Listening at: ${port}`);
});