const mqtt = require('mqtt')
const client  = mqtt.connect('mqtt://ngoinhaiot.com:1111', {
    username: 'duynguyen123',
    password: '4DDF77D157AD4F47'
})
exports.client = client
exports.channel = (topic) =>`duynguyen123/${topic}`