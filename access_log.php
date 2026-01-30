<?php
//Configuration of variables associated with server and database
$servername = "localhost";       // server name
$username   = "rfid_user";       // database user username
$password   = "1234";            // XAMPP password
$dbname     = "rfid_database";   // database name

// Creates new MySQLi object to connect to the database using aobve declared variables
$conn = new mysqli($servername, $username, $password, $dbname);

//Checks if connection failed and stops script execution if so.
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

//Reads input data POSTed to the script from ESP32
$uid    = isset($_POST['uid'])    ? $_POST['uid']    : '';
$status = isset($_POST['status']) ? $_POST['status'] : '';
$key    = isset($_POST['key'])    ? $_POST['key']    : '';

//API key protection enabled so that unauthorized POSTs are blocked (enhances security)
$expected_key = "secret4321"; //Secret key
if ($key !== $expected_key) {
    echo "Forbidden"; //If key does not match, print "Forbidden" to the HTTP response
    $conn->close();   //Close connection to the database
    exit;
}

//Checks that the UID and status fields are not empty strings
if ($uid !== '' && $status !== '') {
    $stmt = $conn->prepare("INSERT INTO access_log (UID, EntryStatus) VALUES (?, ?)"); //Prepares SQL statement
    $stmt->bind_param("ss", $uid, $status); //Binds variables to the above prepared statement (two s's for two variables)
    if ($stmt->execute())
        echo "Success"; //Prints success if execution returns true
    else {
        echo "DB error: "; //Prints this is execution returns false
        $conn->error; //Closes connection as well
    }
    $stmt->close(); //Closes the prepared statement and frees resources
} 
//If UID and status fields are empty, the script prints "Missing params"
else {
    echo "Missing params";
}

$conn->close(); //When the script is finished, the connection is closed
?>
