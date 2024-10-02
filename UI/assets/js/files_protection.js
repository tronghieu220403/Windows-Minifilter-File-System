// Mảng lưu trữ dữ liệu file và thư mục bảo vệ
let protectedFilesAndFolders = [];

// Tải dữ liệu vào bảng khi trang được tải
document.addEventListener('DOMContentLoaded', () => {
    updateProtectedList();
    // setInterval(updateProtectedList, 5000); // Cập nhật danh sách mỗi 5 giây (nếu cần thiết)
});

// Hàm cập nhật dữ liệu từ C++ cho danh sách bảo vệ
function updateProtectedList() {
    protectedFilesAndFolders = GetProtectedList(); // Gọi hàm từ C++ để lấy danh sách file bảo vệ
    loadProtectedTable(); // Cập nhật bảng hiển thị
}

// Hàm tải dữ liệu bảo vệ vào bảng
function loadProtectedTable() {
    const tableBody = document.querySelector('#file-table tbody');
    tableBody.innerHTML = ''; // Xóa nội dung hiện tại trong bảng

    protectedFilesAndFolders.forEach((item, index) => {
        const row = document.createElement('tr');
        row.dataset.index = index;

        // Xử lý khi nhấn vào hàng
        row.onclick = function () {
            this.classList.toggle('selected'); // Thêm hoặc xóa lớp "selected" khi nhấn vào hàng
        };

        // Tạo các ô dữ liệu cho các cột: Path, Type, Status
        const pathCell = document.createElement('td');
        pathCell.textContent = item.path; // Hiển thị đường dẫn
        row.appendChild(pathCell);

        const typeCell = document.createElement('td');
        typeCell.textContent = item.isFile ? 'File' : 'Folder'; // Hiển thị loại: File hoặc Folder
        row.appendChild(typeCell);

        const statusCell = document.createElement('td');
        statusCell.textContent = item.isEnable ? 'Enable' : 'Disable'; // Hiển thị trạng thái: Enable hoặc Disable
        row.appendChild(statusCell);

        tableBody.appendChild(row); // Thêm hàng vào bảng
    });
}

// Hàm thêm file hoặc thư mục cho danh sách bảo vệ
function addProtectedFileFolder(event) {
    if (event.key === 'Enter') { // Kiểm tra nếu nhấn phím Enter
        const input = document.getElementById('file-input');
        const path = input.value.trim();
        if (path) {
            const isFile = !path.endsWith('\\'); // Kiểm tra nếu là file (không kết thúc bằng '\\')
            const isEnable = true; // Mặc định trạng thái là Enable
            protectedFilesAndFolders.push({ path, isFile, isEnable }); // Thêm vào mảng
            AddToProtectedList(path, isFile, isEnable); // Gọi hàm C++ để thêm vào danh sách bảo vệ
            loadProtectedTable(); // Cập nhật bảng
            input.value = ''; // Xóa nội dung trong ô input
        }
    }
}

// Hàm mở hộp thoại chọn file bảo vệ
function selectProtectedFile() {
    const filePath = GetFileName(); // Gọi hàm C++ để mở hộp thoại chọn file
    if (filePath) {
        const isFile = true; // Là file
        const isEnable = true; // Mặc định trạng thái là Enable
        protectedFilesAndFolders.push({ path: filePath, isFile, isEnable });
        AddToProtectedList(filePath, isFile, isEnable); // Gọi hàm C++ để thêm vào danh sách bảo vệ
        loadProtectedTable(); // Cập nhật bảng
    }
}

// Hàm mở hộp thoại chọn thư mục bảo vệ
function selectProtectedFolder() {
    const folderPath = GetFolderName(); // Gọi hàm C++ để mở hộp thoại chọn thư mục
    if (folderPath) {
        const isFile = false; // Là thư mục
        const isEnable = true; // Mặc định trạng thái là Enable
        protectedFilesAndFolders.push({ path: folderPath, isFile, isEnable });
        AddToProtectedList(folderPath, isFile, isEnable); // Gọi hàm C++ để thêm vào danh sách bảo vệ
        loadProtectedTable(); // Cập nhật bảng
    }
}

// Hàm bật bảo vệ cho các hàng được chọn trong danh sách bảo vệ
function enableProtectedProtection() {
    updateProtectedStatusForSelectedRows(true);
}

// Hàm tắt bảo vệ cho các hàng được chọn trong danh sách bảo vệ
function disableProtectedProtection() {
    updateProtectedStatusForSelectedRows(false);
}

// Cập nhật trạng thái bảo vệ cho các hàng được chọn trong danh sách bảo vệ
function updateProtectedStatusForSelectedRows(isEnable) {
    const selectedRows = document.querySelectorAll('#file-table tbody tr.selected'); // Chọn tất cả các hàng đã chọn
    selectedRows.forEach(row => {
        const index = row.dataset.index;
        const file = protectedFilesAndFolders[index];
        file.isEnable = isEnable; // Cập nhật trạng thái trong mảng
        UpdateProtectedStatus(file.path, isEnable); // Gọi hàm C++ để cập nhật trạng thái
    });
    loadProtectedTable(); // Cập nhật bảng
}

// Xóa các hàng được chọn trong danh sách bảo vệ
function deleteProtectedRows() {
    const selectedRows = document.querySelectorAll('#file-table tbody tr.selected'); // Chọn tất cả các hàng đã chọn
    const indexesToRemove = Array.from(selectedRows).map(row => parseInt(row.dataset.index)); // Lấy chỉ mục của các hàng đã chọn
    selectedRows.forEach(row => {
        const index = row.dataset.index;
        const file = protectedFilesAndFolders[index];
        RemoveFromProtectedList(file.path, file.isFile); // Gọi hàm C++ để xóa khỏi danh sách bảo vệ
    });
    protectedFilesAndFolders = protectedFilesAndFolders.filter((_, index) => !indexesToRemove.includes(index)); // Xóa các mục khỏi mảng
    loadProtectedTable(); // Cập nhật bảng
}
