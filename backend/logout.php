<?php
session_start();
require_once '../dB/conn.php';

if (isset($_SESSION['user']) || isset($_SESSION['admin'])) {
    if (isset($_SESSION['user'])) {
        // Update user status to 'inactive'
        $sqlUpdateStatus = "UPDATE member SET status='inactive' WHERE mem_id=?";
        $stmt = $conn->prepare($sqlUpdateStatus);
        $stmt->execute([$_SESSION['user']]);
        unset($_SESSION['user']);
    }

    if (isset($_SESSION['admin'])) {
        // Update admin status to 'inactive'
        $sqlUpdateAdminStatus = "UPDATE member SET status='inactive' WHERE username=?";
        $stmt = $conn->prepare($sqlUpdateAdminStatus);
        $stmt->execute(['Pam']);
        unset($_SESSION['admin']);
    }

    session_destroy();
}

echo "<script>
        alert('Logout successful!');
        window.location.href = '../index.php';
      </script>";
?>