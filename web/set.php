<?php
if (!function_exists('file_put_contents')) {
    function file_put_contents($filename, $data) {
        $f = @fopen($filename, 'w');
        if (!$f) {
            return false;
        } else {
            $bytes = fwrite($f, $data);
            fclose($f);
            return $bytes;
        }
    }
}

$file = "dataset.json";
$textVar = file_get_contents("php://input");
file_put_contents($file, $textVar);
$soumission = "ok";
?>
