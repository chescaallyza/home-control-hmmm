<?php
session_start();
require_once '../dB/conn.php';

if (isset($_POST['action']) && isset($_POST['user_id'])) {
    $userId = $_POST['user_id'];
    $action = $_POST['action'];

    try {
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

        if ($action === 'approve') {
            // Approve the user
            $newStatus = 'inactive';
            $sql = "UPDATE member SET status = :status WHERE mem_id = :id";
            $stmt = $conn->prepare($sql);
            $stmt->bindParam(':status', $newStatus);
            $stmt->bindParam(':id', $userId);
            $stmt->execute();

            $_SESSION['message'] = [
                'text' => 'User approved successfully.',
                'alert' => 'success'
            ];
        } elseif ($action === 'reject') {
            // Delete the user
            $sql = "DELETE FROM member WHERE mem_id = :id";
            $stmt = $conn->prepare($sql);
            $stmt->bindParam(':id', $userId);
            $stmt->execute();

            $_SESSION['message'] = [
                'text' => 'User rejected and deleted successfully.',
                'alert' => 'success'
            ];
        }

        $conn = null;
        header('Location: ../front-end/control.php');
        exit();
    } catch (PDOException $e) {
        echo "Error: " . $e->getMessage();
    }
} else {
    header('Location: ../front-end/control.php');
    exit();
}
