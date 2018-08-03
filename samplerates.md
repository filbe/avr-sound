<!DOCTYPE html>
<html>
<head>
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
	<title></title>
</head>
<body>
	<h1>Sample rate list by frequencies</h1>
	The following sample rates are allowed with following frequencies. Please edit f_cpus table to get more clock rates!<br>
	Please notice that the lower the clock frequency, the lower the maximum reasonable samplerate.<br>
	Notice also the tolerance variable. There is no noticeable difference in sound when the error is below 0.5%, but you decide and edit freely!
	<br><br>
	<h2>Sample rates available in avr-sound - partly tested</h2>
<script>
	var c=1;
	var f_cpus = [1000000, 8000000, 16000000, 20000000, 22000000, 25000000, 30000000];
	var prescaler = 8;
	var tolerance = 0.005;
	var minimum_value = 300;

	for (var b=0;b<f_cpus.length;b++) {
		var theoretical_max = 40000/16000000;
		$("body").append("<div style='display:inline-block;vertical-align:top;width:150px;' class='fcpu"+f_cpus[b]+"'><div><b>F: "+((f_cpus[b]+" ").replace("000000 ","MHz"))+"</b></div></div>");
		var used = [];
		for (var i=0;i<65535;i++) {
			if (f_cpus[b]/prescaler/i - parseInt(f_cpus[b]/prescaler/i) < tolerance && f_cpus[b]/prescaler/i < theoretical_max*f_cpus[b]) {
				if (!used[parseInt(f_cpus[b]/prescaler/i)] && parseInt(f_cpus[b]/prescaler/i) >= minimum_value) {
					$(".fcpu"+f_cpus[b]).append("S: "+parseInt(f_cpus[b]/prescaler/i)+"<br>");
					c++;
					used[parseInt(f_cpus[b]/prescaler/i)] = true;
				}
			}
		}
	}
</script>
</body>
</html>
