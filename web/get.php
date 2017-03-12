<?php
include("JSON.php");
// Future-friendly json_encode
if( !function_exists('json_encode') ) {
    function json_encode($data) {
        $json = new Services_JSON();
        return( $json->encode($data) );
    }
}
// Future-friendly json_decode
if( !function_exists('json_decode') ) {
    function json_decode($data) {
        $json = new Services_JSON();
        return( $json->decode($data) );
    }
}
if( !function_exists('get_file_contents') ) {
	function get_file_contents($filename)
	/* Returns the contents of file name passed
	*/
	{
	$fhandle = fopen($filename, "r");
	$fcontents = fread($fhandle, filesize($filename));
	fclose($fhandle);
	return $fcontents;
	}
}
$file = "data.json";
$json = json_decode(file_get_contents($file),TRUE);
header('Content-Type: application/json');
echo json_encode($json);
?>