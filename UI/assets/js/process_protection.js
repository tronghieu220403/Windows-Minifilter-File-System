// Mảng lưu trữ dữ liệu tiến trình
let processes = [];
let availableProcesses = []; // Danh sách các process để chọn

// Hàm tải dữ liệu vào bảng từ C++
function loadTableFromCpp() {
    const processList = GetProctectedProcessList(); // Lấy danh sách tiến trình từ C++
    processes = processList.map(process => ({
        name: process.path.split('\\').pop(),
        path: process.path,
        status: process.isEnable ? 'Enable' : 'Disable'
    }));

    const tableBody = document.querySelector('#process-table tbody');
    tableBody.innerHTML = '';

    processes.forEach((item, index) => {
        const row = document.createElement('tr');
        row.dataset.index = index;

        // Xử lý khi nhấn vào hàng
        row.onclick = function () {
            this.classList.toggle('selected');
        };

        const nameCell = document.createElement('td');
        nameCell.textContent = item.name;
        row.appendChild(nameCell);

        const pathCell = document.createElement('td');
        pathCell.textContent = item.path;
        row.appendChild(pathCell);

        const statusCell = document.createElement('td');
        statusCell.textContent = item.status;
        row.appendChild(statusCell);

        tableBody.appendChild(row);
    });
}

// Hàm thêm tiến trình
function addProcess(event) {
    if (event.key === 'Enter') {
        addProcessManually();
    }
}

function addProcessManually() {
    const input = document.getElementById('process-input');
    const path = input.value.trim();
    if (path) {
        const result = AddToProctectedProcessList(path, true); // Thêm tiến trình vào C++
        if (result === "Process added successfully") {
            loadTableFromCpp(); // Tải lại bảng sau khi thêm
        } else {
            alert("Failed to add process: " + result);
        }
        input.value = ''; // Xóa nội dung input sau khi thêm
    }
}

// Hàm mở hộp thoại chọn tiến trình
function selectProcess() {
    toggleProcessList(); // Hiển thị hoặc ẩn div chọn process
    if (!document.querySelector('#process-list-container').classList.contains('visible')) {
        const processList = GetProcessList(); // Gọi hàm C++ để lấy danh sách process
        availableProcesses = processList; // Lưu danh sách process để dùng sau
        const processListContainer = document.getElementById('process-list');
        processListContainer.innerHTML = '';

        processList.forEach((process, index) => {
            const item = document.createElement('div');
            item.classList.add('process-item');
            item.textContent = `${process.shortName} (${process.fullPath})`;
            item.dataset.index = index;
            item.onclick = function () {
                selectProcessItem(this);
            };
            processListContainer.appendChild(item);
        });
    }
}

// Hàm chọn một process từ danh sách
function selectProcessItem(element) {
    const selected = document.querySelectorAll('.process-item.selected');
    selected.forEach(item => item.classList.remove('selected')); // Xóa class 'selected' khỏi các item khác
    element.classList.add('selected'); // Thêm class 'selected' vào item hiện tại
}

// Hàm thêm tiến trình từ lựa chọn trong danh sách process
function addProcessFromSelection() {
    const selectedProcess = document.querySelector('.process-item.selected');
    if (selectedProcess) {
        const index = selectedProcess.dataset.index;
        const process = availableProcesses[index];
        const result = AddToProctectedProcessList(process.fullPath, true); // Thêm tiến trình vào C++
        if (result === "Process added successfully") {
            loadTableFromCpp(); // Tải lại bảng sau khi thêm
            toggleProcessList(); // Ẩn danh sách process
        } else {
            alert("Failed to add process: " + result);
        }
    } else {
        alert("Please select a process.");
    }
}

// Hàm mở hộp thoại chọn đường dẫn tiến trình
function selectProcessPath() {
    const filePath = GetExePath(); // Gọi hàm C++ để mở hộp thoại chọn file .exe
    if (filePath && filePath.endsWith('.exe')) {
        const result = AddToProctectedProcessList(filePath, true); // Thêm tiến trình vào C++
        if (result === "Process added successfully") {
            loadTableFromCpp(); // Tải lại bảng sau khi thêm
        } else {
            alert("Failed to add process: " + result);
        }
    } else {
        alert("Please select a valid .exe file.");
    }
}

// Bật hoặc tắt danh sách process và gọi hàm C++ mỗi khi hiển thị
function toggleProcessList() {
    const processListContainer = document.getElementById('process-list-container');
    if (!processListContainer.classList.contains('visible')) {
        const processList = GetProcessList(); // Gọi lại hàm C++ để lấy danh sách process mới
        availableProcesses = processList; // Lưu danh sách process để dùng sau
        const processListContainer = document.getElementById('process-list');
        processListContainer.innerHTML = '';

        processList.forEach((process, index) => {
            const item = document.createElement('div');
            item.classList.add('process-item');
            item.textContent = `${process.shortName} (${process.fullPath})`;
            item.dataset.index = index;
            item.onclick = function () {
                selectProcessItem(this);
            };
            processListContainer.appendChild(item);
        });
    }
    processListContainer.classList.toggle('visible'); // Thêm hoặc xóa class 'visible' để hiển thị hoặc ẩn div
}

// Hàm bật bảo vệ cho các hàng được chọn
function enableProtection() {
    updateStatusForSelectedRows('Enable');
}

// Hàm tắt bảo vệ cho các hàng được chọn
function disableProtection() {
    updateStatusForSelectedRows('Disable');
}

// Cập nhật trạng thái bảo vệ cho các hàng được chọn
function updateStatusForSelectedRows(newStatus) {
    const selectedRows = document.querySelectorAll('#process-table tbody tr.selected');
    selectedRows.forEach(row => {
        const index = row.dataset.index;
        const process = processes[index];
        const enableStatus = (newStatus === 'Enable');
        const result = UpdateProtectedProcessStatus(process.path, enableStatus); // Cập nhật trạng thái từ C++
        if (result !== "Status updated successfully") {
            alert("Failed to update status: " + result);
        } else {
            processes[index].status = newStatus; // Cập nhật trạng thái trong mảng dữ liệu
        }
    });
    loadTableFromCpp(); // Tải lại bảng sau khi cập nhật trạng thái
}

// Tải dữ liệu vào bảng khi trang được tải
document.addEventListener('DOMContentLoaded', loadTableFromCpp);
