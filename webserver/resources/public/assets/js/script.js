
$(document).ready(function() {
    const client = mqtt.connect('ws://ngoinhaiot.com:2222', {
        username:'duynguyen123',
        password: '4DDF77D157AD4F47'
    })
    client.on('connect', ()=>{
        console.log('Connect success')
        client.subscribe('duynguyen123/update')
        setInterval(() => {
            client.publish('duynguyen123/plant', 'UPDATE')
        }, 5000);
    })
    client.on('message', (topic, message)=>{
        topic = `${topic}`.substr("duynguyen123/".length)
        // console.log(topic)
        switch(topic){
            case "update":{
                const { hum, temp, soilTemp, soilMoister, lamp, pump } = JSON.parse(message.toString());
                $('#soil-hum').text(soilMoister);
                $('#air-hum').text(hum)
                $('#airt-temp').text(temp)
                $('#soil-temp').text(soilTemp)
            }
        }
    })
    $.ajax({url: "/time", success: function(result){
        $.ajax({
            url : "/schedule",
            type : "post",
            dataType:"text",
            data : {
                result
            },
            success : function (result){
                console.log("Lapaj lich thanh cong")
            }   
        });
        result.forEach(element => {
            $("ul.ListTime").append(`<li class="TimeItem">
            <p class="TimeItem-Text">Hàng ngày, ${element.stamp}</p>
            <button type="button" data-id="${element._id}" class="Delete-TimeItem"><i class="fas fa-trash-alt"></i></button>
        </li>`);
        });
        $('.Delete-TimeItem').click(function() {
            const id = $(this).attr("data-id");
            const check = confirm("Bạn chắc chắn muốn xóa ???");
            if(!check) {
                return;
            }
            $.ajax({
                url : "/time",
                type : "delete",
                dataType:"text",
                data : {
                    id
                },
                success : function (result){
                    location.reload();
                }   
            });
        });

    }});
    $('#pump-btn').click(function() {
        $.ajax({url: "/pump", success: function(result){

        }});
    })
    $('#lamp-btn').click(function() {
        $.ajax({url: "/lamp", success: function(result){

        }});
    });
    $('#btn30').click(function() {
        let soil = $('#soil').val();
        if(soil < 0 || soil >= 100) {
            alert("Độ ẩm trong khoảng từ 0 - 100");
            return;
        } 
        $.ajax({
            url : "/soil",
            type : "post",
            dataType:"text",
            data : {
                soil: soil
            },
            success : function (result){
                if(result == "success") {
                    alert("Đã cập nhật giới hạn nhiệt độ");
                    $('#soil').text(soil);
                }

            }   
        });
    });
    $('#submit-form').click(function() {
        const time = $('#time-value').val();

        $.ajax({
            url : "/time",
            type : "post",
            dataType:"text",
            data : {
                time
            },
            success : function (result){
                if(result == "dup") {
                    alert("Thời gian đã tồn tại, vui lòng kiểm tra lại !!!");
                    return;
                }
                if(result == "success") {
                    alert("Thêm thời gian thành công");
                    location.reload();
                }

            }   
        });
    });
    

});