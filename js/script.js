document.addEventListener('DOMContentLoaded', () => {
    const containers = document.querySelectorAll('.icon-container');
    const username = sessionStorage.getItem('username');

    // Function to update the UI based on fetched states
    function updateUI(data) {
        containers.forEach(container => {
            const roomNumber = container.getAttribute('data-room');
            if (data[roomNumber]) {
                const state = data[roomNumber].state;

                // Update the data-status attribute and toggle the active class
                const newState = state === 1 ? 'on' : 'off';
                container.setAttribute('data-status', newState);
                container.classList.toggle('active', state === 1);

                // Update the status text
                const statusText = state === 1 ? 'ON' : 'OFF';
                container.querySelector('.status-text').textContent = statusText;
            }
        });
    }

    // Function to fetch the current states from the server
    function fetchStates() {
        fetch('../dB/states.php')
            .then(response => response.json())
            .then(data => {
                if (data.error) {
                    console.error(data.error);
                    return;
                }
                updateUI(data);
            })
            .catch(error => console.error('Error fetching states:', error));
    }

    // Fetch the states initially and then periodically
    fetchStates();
    setInterval(fetchStates, 200); // Adjust the interval as needed (500ms = .5 seconds)

    containers.forEach(container => {
        container.addEventListener('click', () => {
            const isOff = container.getAttribute('data-status') === 'off';
            const newState = isOff ? 'on' : 'off';
            container.setAttribute('data-status', newState);
            container.classList.toggle('active');

            const statusText = isOff ? 'ON' : 'OFF';
            container.querySelector('.status-text').textContent = statusText;

            const roomNumber = container.getAttribute('data-room');
            
            // Send data to control.php
            fetch('../control/control.php', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: 'room=' + encodeURIComponent(roomNumber) + '&state=' + encodeURIComponent(newState)
            })
            .then(response => response.text())
            .then(data => {
                console.log(data); // Handle the response from the PHP script
            })
            .catch(error => {
                console.error('Error:', error);
            });

            // Add notification to local storage
            const itemName = container.querySelector('p').textContent;
            addNotification(`${itemName} turned ${statusText} by ${username}`);
        });
    });

    function addNotification(message) {
        const notifications = JSON.parse(localStorage.getItem('notifications')) || [];
        const timestamp = new Date().toLocaleString();
        notifications.push({ message, timestamp });
        localStorage.setItem('notifications', JSON.stringify(notifications));
    }
});
