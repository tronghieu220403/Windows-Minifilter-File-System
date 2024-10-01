
document.addEventListener("DOMContentLoaded", () => {
    const featureSwitches = document.querySelectorAll(".feature .switch input[type='checkbox']");

    featureSwitches.forEach((switchElement) => {
        switchElement.addEventListener("change", (event) => {
            const featureLabel = event.target.closest(".feature").querySelector(".feature-label").textContent;
            const status = event.target.checked ? "on" : "off";
            
            // Gọi hàm UpdateFeatureStatus từ C++ với tên chức năng và trạng thái
            window.UpdateFeatureStatus(featureLabel, status);
        });
    });
});
