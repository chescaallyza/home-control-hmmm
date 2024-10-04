<?php
session_start();
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SmartHome Control</title>
    <link rel="stylesheet" href="css/indexstyle.css">
</head>
<body>

    <div class="container">
        <nav>
        <h2>Home Automation</h2>
        <ul>
        <div class="buttons">
            <li><a class="button sign-up" href="#" id="signUpBtn">Sign Up</a></li>
            <li><a class="button log-in" href="#" id="logInBtn">Login</a></li>
        </ul>
    </nav>

    <?php if(isset($_SESSION['message'])): ?>
        <script>
            alert("<?php echo $_SESSION['message']['text'] ?>");
        </script>
        <?php 
            unset($_SESSION['message']);
        ?>
    <?php endif; ?>

    <div id="signUpForm" class="form-container">
        <form action="./backend/register_query.php" method="POST" class="signUpFormContent">
            <h3>SIGN UP</h3>
            <input type="text" placeholder="Username" name="username" required>
            <input type="password" placeholder="Password" name="password" required>
            <button type="submit" name="register">Sign Up</button>
        </form>
    </div>
    
    <div id="logInForm" class="form-container">
        <form action="./backend/login_query.php" method="POST" id="logInFormContent">
            <h3>LOGIN</h3>
            <input type="text" placeholder="Username" name="username" required>
            <input type="password" placeholder="Password" name="password" required>
            <button type="submit" name="login">Login</button>
        </form>
    </div>

    <script src="js/clickable.js"></script>
</body>
</html>