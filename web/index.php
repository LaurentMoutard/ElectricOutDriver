<?php

/*
  Read the content of a Json file and write a Json after posting some data
  by L.Moutard
  This example code is in the public domain :-)
  modified 05 mars 2017
*/

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
// Future-friendly file_put_contents
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

$file = "data.json";
$textVar = file_get_contents($file);
//print_r($textVar);
$myJson = (array) json_decode($textVar);

if ($_SERVER["REQUEST_METHOD"] == "POST") {

	//var_dump($_POST);
  if (!empty($_POST["programme"]) || $_POST["programme"] == '0') {
	// echo("prog : ".$_POST["programme"]);
    $myJson["programme"] = $_POST["programme"];
  }
  if (!empty($_POST["delai"])) {
	 //echo("delai : ".$_POST["delai"]);
    $myJson["delai"] = $_POST["delai"];
  }
	$textVar = json_encode($myJson);
	file_put_contents($file, $textVar);
	$soumission = "ok";
}

?>

<!DOCTYPE html>
<html lang="fr">
<head>

  <!-- Basic Page Needs
  –––––––––––––––––––––––––––––––––––––––––––––––––– -->
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<meta charset="utf-8">
	<title>ElectricOutDriver</title>
	<meta name="description" content="Driver for connected electrical outlet">
	<meta name="author" content="Laurent Moutard">
	<meta http-equiv='cache-control' content='no-cache'>
	<meta http-equiv='expires' content='0'>
	<meta http-equiv='pragma' content='no-cache'>
  <!-- Mobile Specific Metas
  –––––––––––––––––––––––––––––––––––––––––––––––––– -->
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <!-- FONT
  –––––––––––––––––––––––––––––––––––––––––––––––––– -->
  <link href="//fonts.googleapis.com/css?family=Raleway:400,300,600" rel="stylesheet" type="text/css">

  <!-- CSS
  –––––––––––––––––––––––––––––––––––––––––––––––––– -->
  <link rel="stylesheet" href="css/normalize.css">
  <link rel="stylesheet" href="css/skeleton.css">

  <!-- Favicon
  –––––––––––––––––––––––––––––––––––––––––––––––––– -->
  <link rel="icon" type="image/png" href="images/favicon.png">
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js"></script>
<script>

$(document).ready(function(){
	getFromElectricDriver();
	var refreshId = setInterval("getFromElectricDriver()", 2000);
}); 

function getFromElectricDriver(){
	$("#DateModified").text(Date());
	$.getJSON('dataset.json', function(data) {
	  $.each( data, function( key, val ) {
		if (key == "programmeEnCours"){
			$("#programmeEnCours").text(val);
		}
		if (key == "delaiEnCours"){
			$("#delaiEnCours").text(val);
		}
	  });
	});
}

</script>
</head>
<body>

  <!-- Primary Page Layout
  –––––––––––––––––––––––––––––––––––––––––––––––––– -->
  <div class="container">
    <div class="row">
      <div class="one-half column" style="margin-top: 25%">
        <h4>ElectricOutDriver</h4>
      </div>
    </div>
    <div class="row">
	  <ul>
		  <li>
			<div id = "DateModified" style="display:inline;"></div>
		  </li>
		  <li>
			Programme En Cours : <div id = "programmeEnCours" style="display:inline;"></div>
		</li>
		  <li>
			Délai en cours :  <div id = "delaiEnCours" style="display:inline;"></div>
		</li>
    </ul>
    </div>
	<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>" method="POST">
	  <div class="row">
		<div class="six columns">
		  <label for="delai">Délai</label>
		  <input class="u-pull-left" value="<?php echo($myJson["delai"])?>" id="delai" name="delai" type="number">
		</div>
		<div class="six columns">
		  <label for="programme">Programme</label>
		  <select class="u-full-width" id="programme" name="programme">
			<option value="0" <?php if ($myJson["programme"] == "0") echo "selected = 'selected'";?>>Tous éteints</option>
			<option value="1" <?php if ($myJson["programme"] == "1") echo "selected = 'selected'";?>>Inter 1</option>
			<option value="2" <?php if ($myJson["programme"] == "2") echo "selected = 'selected'";?>>Inter 2</option>
			<option value="3" <?php if ($myJson["programme"] == "3") echo "selected = 'selected'";?>>Inter 3</option>
			<option value="4" <?php if ($myJson["programme"] == "4") echo "selected = 'selected'";?>>Inter 1+2</option>
			<option value="5" <?php if ($myJson["programme"] == "5") echo "selected = 'selected'";?>>Inter 1+3</option>
			<option value="6" <?php if ($myJson["programme"] == "6") echo "selected = 'selected'";?>>Inter 2+3</option>
			<option value="7" <?php if ($myJson["programme"] == "7") echo "selected = 'selected'";?>>Inter 1+2+3</option>
			<option value="8" <?php if ($myJson["programme"] == "8") echo "selected = 'selected'";?>>Chenillard</option>
			<option value="9" <?php if ($myJson["programme"] == "9") echo "selected = 'selected'";?>>Aléatoire</option>
		  </select>
		</div>
	  </div>
	  <input class="button-primary u-pull-right" value="Submit" type="submit">
	</form>
<!-- Always wrap checkbox and radio inputs in a label and use a <span class="label-body"> inside of it -->

<!-- Note: The class .u-full-width is just a utility class shorthand for width: 100% -->	  </div>

<!-- End Document
  –––––––––––––––––––––––––––––––––––––––––––––––––– -->
</body>
</html>
