
// Hàm load trang con trong settings
function loadSubPage(subPage) {
    document.getElementById('settings-iframe').src = subPage;

    // Cập nhật trạng thái active cho settings-menu-item
    const menuItems = document.querySelectorAll('.settings-menu-item');
    menuItems.forEach(item => {
        item.classList.remove('active');
    });

    // Tìm và đánh dấu mục active dựa trên subPage
    menuItems.forEach(item => {
        if (item.getAttribute('onclick').includes(subPage)) {
            item.classList.add('active');
        }
    });
}

// Đảm bảo trang con đầu tiên được load khi mở settings
document.addEventListener('DOMContentLoaded', () => {
    loadSubPage('settings_trusted_process.html');
});
