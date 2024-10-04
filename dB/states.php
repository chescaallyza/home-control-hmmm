<?php
header('Content-Type: application/json');

// Include the database connection file
require_once './conn.php';

try {
    // Prepare and execute SQL query
    $stmt = $conn->prepare('SELECT room, state FROM states');
    $stmt->execute();

    // Fetch all results
    $rows = $stmt->fetchAll(PDO::FETCH_ASSOC);

    // Convert results to a key-value array
    $data = array();
    foreach ($rows as $row) {
        $data[$row['room']] = array(
            'room' => $row['room'],
            'state'  => $row['state']
        );
    }

    // Output data in JSON format
    echo json_encode($data);

} catch (PDOException $e) {
    echo json_encode(array("error" => $e->getMessage()));
}
?>
