<?php
session_start();
require_once '../dB/conn.php';

if (isset($_POST['login'])) {
    if (!empty($_POST['username']) && !empty($_POST['password'])) {
        $username = $_POST['username'];
        $password = $_POST['password'];

        if ($username == 'Pam' && $password == '123456') {
            // Admin login successful
            $_SESSION['admin'] = true;
            $_SESSION['username'] = $username; // Store the username in the session

            // Update admin status to 'active'
            $sqlUpdateAdminStatus = "UPDATE member SET status='active' WHERE username=?";
            $stmt = $conn->prepare($sqlUpdateAdminStatus);
            $stmt->execute([$username]);

            echo "<script>
                    sessionStorage.setItem('username', '" . addslashes($username) . "'); // Store username in sessionStorage
                    alert('Admin login successful!');
                    window.location.href = '../front-end/admin.php';
                  </script>";
            exit();
        }

        $sql = "SELECT * FROM member WHERE username=? AND password=?";
        $query = $conn->prepare($sql);
        $query->execute([$username, $password]);
        $row = $query->rowCount();
        $fetch = $query->fetch();

        if ($row > 0) {
            if ($fetch['status'] === 'pending') {
                echo "<script>
                        alert('Your account is still pending approval.');
                        window.location.href = '../index.php';
                      </script>";
            } else {
                $_SESSION['user'] = $fetch['mem_id'];
                $_SESSION['username'] = $username; // Store the username in the session

                $sqlUpdateStatus = "UPDATE member SET status='active' WHERE mem_id=?";
                $stmt = $conn->prepare($sqlUpdateStatus);
                $stmt->execute([$fetch['mem_id']]);

                echo "<script>
                        sessionStorage.setItem('username', '" . addslashes($username) . "'); // Store username in sessionStorage
                        alert('Login successful!');
                        window.location.href = '../front-end/home.php';
                      </script>";
            }
        } else {
            echo "<script>
                    alert('Login failed!');
                    window.location.href = '../index.php';
                  </script>";
        }
    } else {
        echo "<script>
                alert('Username or Password field is empty!');
                window.location.href = '../index.php';
              </script>";
    }
}
?>
