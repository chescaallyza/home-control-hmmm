<!DOCTYPE html>
<?php
require '../dB/conn.php'; // connecting to dB
session_start();

// Fetch the count of pending registrations
$sql = "SELECT COUNT(*) as pending_count FROM member WHERE status = 'pending'";
$stmt = $conn->prepare($sql);
$stmt->execute();
$pendingCount = $stmt->fetch(PDO::FETCH_ASSOC)['pending_count'];

// Fetch the pending registration details
$sql = "SELECT * FROM member WHERE status = 'pending'";
$stmt = $conn->prepare($sql);
$stmt->execute();
$pendingUsers = $stmt->fetchAll(PDO::FETCH_ASSOC);

// Debugging output
// print_r($pendingUsers); // Uncomment this line to see the raw output
?>
<html lang="en">
<head>
    <title>Control Logs</title>
    <link rel="stylesheet" href="../css/style.css">
    <link rel="stylesheet" href="../css/admin-approval.css">
</head>
<body>
    <nav>
        <h2>Home Automation</h2>
        <ul>
            <li><a href="admin.php">Home</a></li>
            <li><a href="#">Control Logs<?php if ($pendingCount > 0) { echo " ($pendingCount)"; } ?></a></li>
            <li><a href="../backend/logout.php">Logout</a></li>
        </ul>
    </nav>

    <header class="header-text">
        <h1>Pending Registrations</h1>
    </header>

    <section class="admin-approval-section">
        <?php
        if (!empty($pendingUsers)) {
            echo '<table>';
            echo '<tr><th>Username</th><th>Action</th></tr>';
            foreach ($pendingUsers as $user) {
                echo '<tr>';
                echo '<td>' . htmlspecialchars($user['username']) . '</td>';
                // Check if 'mem_id' key exists before accessing
                if (isset($user['mem_id'])) {
                    echo '<td>
                            <form method="post" action="../backend/approve_reject.php">
                                <input type="hidden" name="user_id" value="' . htmlspecialchars($user['mem_id']) . '">
                                <button type="submit" name="action" value="approve">Approve</button>
                                <button type="submit" name="action" value="reject">Reject</button>
                            </form>
                          </td>';
                } else {
                    echo '<td>Error: User ID not found</td>';
                }
                echo '</tr>';
            }
            echo '</table>';
        } else {
            echo '<p>No pending registrations.</p>';
        }
        ?>
    </section>

    <header class="header-text">
        <h1>Notifications</h1>
    </header>

    <section class="notifications-section">
        <table id="notifications-table">
            <thead>
                <tr>
                    <th>Notification</th>
                    <th>Timestamp</th>
                </tr>
            </thead>
            <tbody id="notifications-list">
                <!-- Notifications will be added here dynamically -->
            </tbody>
        </table>
        <button id="clear-notifications">Clear Notifications</button>
    </section>

    <script src="../js/script.js"></script>
    <script src="../js/notif-script.js"></script>
</body>
</html>
