document.addEventListener('DOMContentLoaded', function() {
    const signUpBtn = document.getElementById('signUpBtn');
    const logInBtn = document.getElementById('logInBtn');
    const userBtn = document.getElementById('userBtn');
    const adminBtn = document.getElementById('adminBtn');
    const signUpForm = document.getElementById('signUpForm');
    const logInForm = document.getElementById('logInForm');
    let currentAction = ''; // Keeps track of whether "Sign Up" or "Login" was clicked

    // Function to hide all forms and show the desired form
    function showForm(formToShow) {
        signUpForm.style.display = 'none';
        logInForm.style.display = 'none';
        formToShow.style.display = 'block';
    }

});

document.getElementById('signUpBtn').addEventListener('click', function() {
    document.getElementById('signUpForm').style.display = 'block';
    document.getElementById('logInForm').style.display = 'none';
});

document.getElementById('logInBtn').addEventListener('click', function() {
    document.getElementById('logInForm').style.display = 'block';
    document.getElementById('signUpForm').style.display = 'none';
});

//Handle Sign Up Form Submission
document.getElementById('.signUpFormContent').addEventListener('submit', function(e) {
    e.preventDefault();
    showForm(logInForm);
    alert("Sign up Successful");
});

function displayMessage(message, isSuccess) {
    // Create the alert box
    const alertBox = document.createElement('div');
    alertBox.classList.add('alert');
    alertBox.textContent = message;

    // Apply styles based on success or failure
    if (isSuccess) {
        alertBox.classList.add('alert-success');
    } else {
        alertBox.classList.add('alert-danger');
    }

    // Append the alert box to the body
    document.body.appendChild(alertBox);

    // Fade out and remove the alert box after a few seconds
    setTimeout(() => {
        alertBox.classList.add('fade-out');
        setTimeout(() => alertBox.remove(), 1000); // Remove element after fade-out
    }, 3000); // Display for 3 seconds
}


// Handle Log In Form Submission
document.getElementById('logInFormContent').addEventListener('submit', function(e) {
    e.preventDefault();
    // Here, you would typically send the form data to the server to authenticate the user
    alert('Login Successful!'); // Replace with actual login logic

    // Create a temporary link element and trigger a click event to redirect
    const tempLink = document.createElement('a');
    tempLink.href = 'front-end/home.php';
    tempLink.style.display = 'none'; // Hide the link element
    document.body.appendChild(tempLink); // Append it to the body
    tempLink.click(); // Trigger the click event
    document.body.removeChild(tempLink); // Clean up by removing the link
});
