<?php
require '../dB/updateStates.php';

header('Content-Type: application/json');

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Check if required parameters are present
    if (isset($_POST['room']) && isset($_POST['state'])) {
        $room = $_POST['room'];
        $state = $_POST['state'] === 'on' ? 1 : 0;

        // Create an instance of RoomControl and update the room state
        $roomControl = new RoomControl($conn);
        $result = $roomControl->updateRoomState($room, $state);

        echo json_encode($result);
    } else {
        echo json_encode([
            'success' => false,
            'message' => 'Invalid parameters'
        ]);
    }
} else {
    echo json_encode([
        'success' => false,
        'message' => 'Invalid request method'
    ]);
}
?>
