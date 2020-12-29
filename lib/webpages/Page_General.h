//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Configuration</strong>
<hr>
<form action="" method="post">
<hr>
<strong>Device Configuration</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
	<td align="right">Username:</td>
	<td><input type="text" id="webusername" name="webusername" value=""></td>
</tr>
<tr>
	<td align="right">Password:</td>
	<td><input type="text" id="webpassword" name="webpassword" value=""></td>
</tr>
<tr><td>Sleep Time:</td><td>
<select  id="sltm" name="sltm">
  <option value="1">1 Min</option>
  <option value="5">5 Min</option>
  <option value="10">10 Min</option>
  <option value="15">15 Min</option>
  <option value="30">30 Min</option>
  <option value="61">1 Hour</option>
  <option value="121">2 Hour</option>
  <option value="181">3 Hour</option>
  <option value="241">4 Hour</option>
</select>
</td></tr>
</table>

<hr>
<strong>Device info</strong><br>
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">sw Version:</td><td><span id="sw_Version"></span></td></tr>
<tr><td align="right">Mac:</td><td><span id="x_mac"></span></td></tr>
<tr><td align="right">AP SSID:</td><td><span id="ap_ssid"></span></td></tr>
<tr><td align="right">Date/Time:</td><td><span id="x_ntp"></span></td></tr>
</table>

<hr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>

</form>
<script>



window.onload = function ()
{
	load("style.css","css", function()
	{
		load("microajax.js","js", function()
		{
				setValues("/admin/generalvalues");
		});
	});
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_general_html()
{

	if (MyWebServer.args() > 0 )  // Save Settings
	{
		for ( uint8_t i = 0; i < MyWebServer.args(); i++ ) {
			if (MyWebServer.argName(i) == "webusername") strcpy(config.WEB_User, urldecode(MyWebServer.arg(i)).c_str());
			if (MyWebServer.argName(i) == "webpassword" && urldecode(MyWebServer.arg(i)) != "") strcpy(config.WEB_Password, urldecode(MyWebServer.arg(i)).c_str());
			if (MyWebServer.argName(i) == "sltm") config.SLEEPTime =  MyWebServer.arg(i).toInt(); 
		}
		storage_write();
		firstStart = true;
	}
	if (!MyWebServer.authenticate(config.WEB_User, config.WEB_Password))  {
		MyWebServer.requestAuthentication();
	}
	else{
		MyWebServer.send ( 200, "text/html", PAGE_AdminGeneralSettings );
	}
	MyWebServer.send ( 400, "text/html", "Authentication failed" );
	Serial.println(__FUNCTION__);


}

void send_general_configuration_values_html()
{
	curDateTime();
	String values ="";
	values += "webusername|" +  String(config.WEB_User) +  "|input\n";
	//values += "webpassword|" +  String(config.WEB_Password) +  "|input\n";  	// KEEP IT COMMENTED TO NOT SHOW THE WiFi KEY!!!
	values += "sltm|" + (String) config.SLEEPTime + "|input\n";
 	values += "sw_Version|" + String(SWVer) +  "|div\n";
 	values += "x_mac|" + GetMacAddress() +  "|div\n";
    values += "ap_ssid|" + ESP_SSID +  "|div\n";
    values += "x_ntp|" + (String) WeekDays[DateTime.wday] + ", " + (String) DateTime.year + "/" + (String) DateTime.month + "/" + (String) DateTime.day + "   " + (String) DateTime.hour + ":" + (String) DateTime.minute + ":" + (String)  DateTime.second + "|div\n";
	MyWebServer.send ( 200, "text/plain", values);
	Serial.println(__FUNCTION__);
}
