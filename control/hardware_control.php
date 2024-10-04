<?php
// This file handles both user actions and hardware updates to control the state (0/1) and timestamp in the database

require '../dB/conn.php'; // Include your database connection file

$lock_file = '../dB/control.lock';

function lock() {
    global $lock_file;
    file_put_contents($lock_file, "1");
}

function unlock() {
    global $lock_file;
    if (file_exists($lock_file)) {
        unlink($lock_file);
    }
}

function is_locked() {
    global $lock_file;
    return file_exists($lock_file);
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get the raw POST data
    $data = file_get_contents('php://input');
    $json = json_decode($data, true);

    // Check if the request is to toggle room status
    if (isset($json['room']) && isset($json['toggle'])) {
        $room = intval($json['room']);

        // Wait if the lock is active
        while (is_locked()) {
            sleep(1); // Wait 1 second before trying again
        }

        lock(); // Lock the control

        try {
            // Fetch the current state of the room
            $stmt = $conn->prepare("SELECT state FROM states WHERE room = :room");
            $stmt->bindParam(':room', $room, PDO::PARAM_INT);
            $stmt->execute();
            $current_state = $stmt->fetchColumn();

            // Toggle the state
            $new_state = $current_state == 1 ? 0 : 1;

            // Prepare the SQL update statement
            $sql = "UPDATE states SET state = :state, last_updated = CURRENT_TIMESTAMP WHERE room = :room";
            $stmt = $conn->prepare($sql);

            // Bind parameters
            $stmt->bindParam(':room', $room, PDO::PARAM_INT);
            $stmt->bindParam(':state', $new_state, PDO::PARAM_INT);

            // Execute the statement
            if ($stmt->execute()) {
                echo json_encode(['success' => true, 'message' => 'Status updated successfully.']);
            } else {
                echo json_encode(['success' => false, 'message' => 'Failed to update status.']);
            }
        } catch (PDOException $e) {
            echo json_encode(['success' => false, 'message' => 'Error: ' . $e->getMessage()]);
        } finally {
            unlock(); // Unlock the control
        }
    } else {
        echo json_encode(['success' => false, 'message' => 'Invalid request.']);
    }
} else {
    echo json_encode(['success' => false, 'message' => 'Invalid request method.']);
}
?>
