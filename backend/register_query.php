<?php
session_start();
require_once '../dB/conn.php';

if (isset($_POST['register'])) {
    if (!empty($_POST['username']) && !empty($_POST['password'])) {
        try {
            $username = $_POST['username'];
            $password = $_POST['password'];
            $role = 'user';
            $status = 'pending';  // Set default status to pending

            $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

            // Prepare SQL statement with named placeholders
            $sql = "INSERT INTO member (username, password, role, status) VALUES (:username, :password, :role, :status)";
            $stmt = $conn->prepare($sql);

            // Bind parameters
            $stmt->bindParam(':username', $username);
            $stmt->bindParam(':password', $password);
            $stmt->bindParam(':role', $role);
            $stmt->bindParam(':status', $status);

            // Execute the statement
            $stmt->execute();

            // Set session message for success
            $_SESSION['message'] = [
                'text' => 'Registration successful! Waiting for admin approval.',
                'alert' => 'success'
            ];

            $conn = null;
            header('Location: ../index.php');
            exit();
        } catch (PDOException $e) {
            echo "Error: " . $e->getMessage();
        }
    } else {
        echo "
            <script>alert('Please fill up the required fields!')</script>
            <script>window.location = 'registration.php'</script>
        ";
    }
}
?>
