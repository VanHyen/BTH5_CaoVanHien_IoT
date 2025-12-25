const SERVER_URL = 'http://localhost:3000';
let socket;
let isConnected = false;
let tempChart;

const MAX_DATA_POINTS = 20; // Số lượng điểm hiển thị trên biểu đồ

// Khởi tạo biểu đồ khi trang web load
function initChart() {
    const ctx = document.getElementById('tempChart').getContext('2d');
    tempChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [], // Thời gian
            datasets: [{
                label: 'Nhiệt độ (°C)',
                data: [],
                borderColor: 'rgba(255, 99, 132, 1)',
                backgroundColor: 'rgba(255, 99, 132, 0.2)',
                borderWidth: 2,
                fill: true,
                tension: 0.4
            }]
        },
        options: {
            scales: {
                y: { beginAtZero: false }
            }
        }
    });
}

function updateChart(temp) {
    const now = new Date().toLocaleTimeString();
    
    // Thêm dữ liệu mới
    tempChart.data.labels.push(now);
    tempChart.data.datasets[0].data.push(temp);

    // Nếu quá nhiều điểm, xóa điểm cũ nhất để biểu đồ "trôi"
    if (tempChart.data.labels.length > MAX_DATA_POINTS) {
        tempChart.data.labels.shift();
        tempChart.data.datasets[0].data.shift();
    }

    tempChart.update('none'); // Update mà không cần hiệu ứng chuyển động quá nhiều
}

// Giữ nguyên các hàm connect/disconnect của bạn và sửa socket.on('sensorData')
// ... (phần code cũ của bạn) ...

socket.on('sensorData', (data) => {
    if (isConnected) {
        document.getElementById('temperature').textContent = data.temperature.toFixed(1);
        document.getElementById('humidity').textContent = data.humidity.toFixed(1);
        
        // Gọi hàm cập nhật biểu đồ
        updateChart(data.temperature);
    }
});

// Đừng quên gọi initChart() khi nhấn Connect hoặc khi load trang
initChart();