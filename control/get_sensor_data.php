<?php
header('Content-Type: application/json');

$filename = '../backend/sensor_data.json'; // Path to your JSON file with sensor data

if (file_exists($filename)) {
    $data = json_decode(file_get_contents($filename), true);

    if (json_last_error() === JSON_ERROR_NONE) {
        echo json_encode([
            'temperature' => $data['temperature'] ?? 'N/A',
            'irDetected' => $data['irDetected'] ?? false,
            'light' => $data['light'] ?? 'N/A',
            'lightStatus' => $data['lightStatus'] ?? false
        ]);
    } else {
        echo json_encode(['error' => 'Error decoding JSON']);
    }
} else {
    echo json_encode(['error' => 'File not found']);
}