var http   = require('http');
var Router = require('regex-router');
var SerialPort = require("node-serialport").SerialPort;

require('buffer');

function mod(n, m) {
  return ((n % m) + m) % m;
}

function getBytes( x ){
    var bytes = new Uint8Array(4);
    var i = 0;
    x %= 0x80000000;
    while ( i < 4) {
      bytes[i++] = x & 0xFF;
      x = x>>8;
    }
    return bytes;
}

var appendBuffer = function(buffer1, buffer2) {
  var tmp = new Uint8Array(buffer1.byteLength + buffer2.byteLength);
  tmp.set(new Uint8Array(buffer1), 0);
  tmp.set(new Uint8Array(buffer2), buffer1.byteLength);
  return tmp.buffer;
};

function failReq(res, msg) {
  res.statusCode = 400;
  res.write(msg);
  res.end();
}

var TeensyConnection = function(callback) {
  
  this.is_opened = false;
  var _this = this;

  this.serialPort = new SerialPort("/dev/ttyO3", {
    baudrate: 9600
  }); 

  this.serialPort.on("open", function (error) {
    if ( error ) {
      console.log('Failed to open port: '+error);
    } else {
      console.log('Port opened!');
      _this.is_opened = true;
      _this.serialPort.on('data', function(data) {
        console.log('data received: ' + data);
      });
      callback();
    }
  });
};

TeensyConnection.prototype.send_frame = function(payload) {

  var header    = new Uint8Array(2);
  var frame_end = new Uint8Array(2);

  header[0] = 2;
  header[1] = payload.byteLength;

  frame_end[0] = this.get_checksum(payload);
  frame_end[1] = 3;

  var frame = appendBuffer(header, payload);
  frame = appendBuffer(frame, frame_end);

  console.log(frame);
  
  var buf = new Buffer(frame.byteLength);
  
  for (var i = 0; i < frame.byteLength; i++) {
    buf.writeUInt8(frame[i], i);
  };
  
  this.serialPort.write(buf);

};

TeensyConnection.prototype.get_checksum = function (payload) {
  var checksum = 0;

  for (var i = 0; i < payload.byteLength; i++) {
    checksum = checksum ^ payload[i];
  }

  return checksum;
};

var TeensyMediator = function() {

  var _this = this;

  this.teensy_connection = new TeensyConnection(function() {
    _this.run_anim('comm_test', 
    {
    	"time"  : "2000",
    	"freq"  : "3.0",
    	"color" : "48FFFF",
    	"hsv"   : "yes"
    });
  });

  this.router = new Router(function(req, res, m) {
    res.end('404. URL not found: ' + req.url);
  });

  this.router.post(/^\/leds\/(\w+)/, function(req, res, m) {

    if (m[1] in TeensyMediator.ANIMATIONS) {

      var anim = m[1];

      _this.process_request_data(req, function(options) {
        if (options == null) {
          failReq(res, "Error parsing options!");
          return;
        }

        console.log(options);

        var result = _this.check_valid_options(options);
        if (!result) {
          failReq(res, "Wrong options!");
        } else {
          _this.run_anim(anim, options);
        } 
        res.end();
      });
    }
  })

  http.createServer(function(req, res) {
    _this.router.route(req, res);
  }).listen(8080);

};

TeensyMediator.ANIMATIONS = {
  'boot_anim'    : 0,
  'comm_test'    : 1,
  'random_noise' : 2,
  'no_anim'      : 3
};

TeensyMediator.VALID_ANIM_OPTIONS = ['time', 'freq', 'color', 'seed', 'hsv', 'example_flag', 'example_flag_2'];

TeensyMediator.FLAGS = {
  'hsv'            : 1,
  'example_flag'   : 2,
  'example_flag_2' : 4
};

TeensyMediator.prototype.run_anim = function(anim, options) {
  var anim_num = TeensyMediator.ANIMATIONS[anim];
  
  var freq   = mod(Math.floor((parseFloat(options['freq']) || 1.0) * 1000), 0xFFFF);

  var color  = mod(parseInt(options['color'], 16) || 0,     0x1000000 );
  var seed   = mod(parseInt(options['seed'])      || 0,     0x80000000);
  var time   = mod(parseInt(options['time'])      || 0,     0x80000000);

  var flags = 0;

  for (var flag in TeensyMediator.FLAGS) {
    if (flag in options) {
      var flag_value = options[flag] === "yes" ? TeensyMediator.FLAGS[flag] : 0;
      var flags = flags | flag_value;
    }
  }

  var byte_arr = new Uint8Array(getBytes(anim_num));
  byte_arr = appendBuffer(byte_arr, getBytes(time));
  byte_arr = appendBuffer(byte_arr, getBytes(color));
  byte_arr = appendBuffer(byte_arr, getBytes(freq));
  byte_arr = appendBuffer(byte_arr, getBytes(seed));
  byte_arr = appendBuffer(byte_arr, getBytes(flags));

  console.log(time + " " + color + " " + freq + " " + seed + " " + flags);
  console.log(byte_arr);

  this.teensy_connection.send_frame(byte_arr);
};

TeensyMediator.prototype.parse_request_data = function(data) {
  var items = {};
  var failed = false;

  data.trim().split('&').forEach( function(pair) {
    var pair = pair.trim().split('=');
    
    if (pair.length != 2) { 
      failed = true;
      return;
    }
    
    items[pair[0].trim()] = pair[1].trim();
  });

  if (failed == true) {
    return null;
  }

  return items;
};

TeensyMediator.prototype.process_request_data = function(req, callback) {
  var data  = "";
  var items = {};
  var _this = this;
  req.on('data', function(chunk) {
      data += chunk.toString();
  });

  req.on('end', function() {
      items = _this.parse_request_data(data);
      callback(items);
  });
};

TeensyMediator.prototype.check_valid_options = function(options) {
  for(var key in options) {
    if (TeensyMediator.VALID_ANIM_OPTIONS.indexOf(key) < 0) {
      return false;
    }
  }
  return true;
};

var tc = new TeensyMediator();