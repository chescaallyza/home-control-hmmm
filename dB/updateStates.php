<?php
require 'conn.php';

class RoomControl {
    private $conn;

    public function __construct($conn) {
        $this->conn = $conn;
    }

    public function updateRoomState($room, $state) {
        $query = "UPDATE states SET state = :state WHERE room = :room";
        $stmt = $this->conn->prepare($query);
        $stmt->bindParam(':state', $state);
        $stmt->bindParam(':room', $room);

        if ($stmt->execute()) {
            return [
                'success' => true,
                'message' => "Room $room state updated to $state"
            ];
        } else {
            return [
                'success' => false,
                'message' => "Failed to update room $room state"
            ];
        }
    }
}
