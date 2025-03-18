const fs = require('fs');
const SerialPort = require('serialport').SerialPort;
const { ReadlineParser } = require('@serialport/parser-readline');


const logStream = fs.createWriteStream('uart_log.txt', { flags: 'a' });

const port = new SerialPort({
    path: 'COM3',
  baudRate: 115200,
  autoOpen: false
});

const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }));

port.open((err) => {
    if (err) {
        console.error(`❌ 포트 열기 실패: ${err.message}`);
        return;
    }
    console.log(`✅ 포트 COM3가 성공적으로 열렸습니다.`);
});

parser.on('data', (data) => {
  console.log('수신 데이터:', data);
  logStream.write(`${new Date().toISOString()} - ${data}\n`);
});

port.on('error', (err) => {
  console.error('Serial Port 에러:', err.message);
});
process.stdin.resume();
