const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

// Cấu hình
const HTTP_PORT = 3000;
const PORT_NAME = 'COM2';
const BAUD_RATE = 9600;

const app = express();
const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: "*",
    methods: ["GET", "POST"]
  }
});

let serialPort;
let parser;

// Khởi tạo kết nối Serial
function initSerial() {
  try {
    serialPort = new SerialPort({
      path: PORT_NAME,
      baudRate: BAUD_RATE
    });

    parser = serialPort.pipe(new ReadlineParser({ delimiter: '\n' }));

    serialPort.on('open', () => {
      console.log(`Kết nối Serial thành công tại ${PORT_NAME}`);
    });

    parser.on('data', (data) => {
      try {
        const jsonData = JSON.parse(data.trim());
        console.log('Dữ liệu nhận được:', jsonData);
        
        // Gửi dữ liệu tới tất cả client qua WebSocket
        io.emit('sensorData', {
          temperature: jsonData.temp,
          humidity: jsonData.hum,
          timestamp: Date.now()
        });
      } catch (err) {
        console.error('Lỗi parse JSON:', err.message);
      }
    });

    serialPort.on('error', (err) => {
      console.error('Lỗi Serial:', err.message);
    });

  } catch (err) {
    console.error('Không thể mở cổng Serial:', err.message);
    console.log('Kiểm tra lại tên cổng COM và driver');
  }
}

// Serve static files
app.use(express.static('public'));

// Bắt đầu kết nối
server.listen(HTTP_PORT, () => {
  console.log(`Server đang chạy tại http://localhost:${HTTP_PORT}`);
  initSerial();
});

// Tắt kết nối
process.on('SIGINT', () => {
  console.log('\nĐang tắt server...');
  if (serialPort && serialPort.isOpen) {
    serialPort.close();
  }
  process.exit();
});