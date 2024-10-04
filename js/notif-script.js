document.addEventListener('DOMContentLoaded', () => {
    const notificationsTable = document.getElementById('notifications-table');
    const notificationsList = document.getElementById('notifications-list');
    const clearButton = document.getElementById('clear-notifications');
    const notifications = JSON.parse(localStorage.getItem('notifications')) || [];

    function displayNotifications() {
        notificationsList.innerHTML = '';
        notifications.forEach(notification => {
            const row = document.createElement('tr');
            const notificationCell = document.createElement('td');
            const timestampCell = document.createElement('td');
            notificationCell.textContent = notification.message;
            timestampCell.textContent = notification.timestamp;
            row.appendChild(notificationCell);
            row.appendChild(timestampCell);
            notificationsList.appendChild(row);
        });
    }

    clearButton.addEventListener('click', () => {
        localStorage.removeItem('notifications');
        notificationsList.innerHTML = ''; // Clear the list visually
    });

    displayNotifications();
});
