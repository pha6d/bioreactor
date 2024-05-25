<?php
header('Content-Type: application/json');

$logFile = '/logs/logfile.txt'; // Update with your log file path

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $sensor_value = $_POST['sensor_value'];

    // Log the received data
    file_put_contents($logFile, date('Y-m-d H:i:s') . " - Sensor Value: " . $sensor_value . PHP_EOL, FILE_APPEND);

    echo json_encode([
        "status" => "success",
        "message" => "Data received",
        "sensor_value" => $sensor_value
    ]);
} else {
    echo json_encode([
        "status" => "error",
        "message" => "Only POST method is allowed"
    ]);
}
?>
