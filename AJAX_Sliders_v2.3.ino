#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(301);
const char* ssid="SKY772BB"; // <========================= your SSID
const char* password="AUCQURXA"; // <===================== your Password
String webSite,XML;
unsigned long wait000=0UL,wait001=1000UL;
int LED=4;
int LED2 = 5;
const int sliderMAX=1;                         // This sets the number of sliders you want
int sliderVal[sliderMAX]={60},ESPval[sliderMAX];

void buildWebsite(){
  webSite="<!DOCTYPE HTML>\n";
  webSite+="<META name='viewport' content='width=device-width, initial-scale=1'>\n";

  webSite+= "<style>\r\n";
  webSite+= ".button-red,.button-blue\r\n"; 
  webSite+= "{color: white;\r\n";
  webSite+= "border-radius: 9px;\r\n";
  webSite+= "font-family:Arial;\r\n";
  webSite+= "font-size:20px;\r\n";
  webSite+= "padding:50px 80px;\r\n";
  webSite+= "}\r\n";
  webSite+= ".button-red:hover:active,.button-blue:hover:active\r\n"; 
  webSite+= "{position:relative;top:3px;color: yellow;}\r\n";
  webSite+= ".button-red {background: rgb(202, 60, 60);}\r\n";
  webSite+= ".button-blue {background: rgb(100,116,255);}\r\n";
  webSite+= "h1{color:black;font-family:Arial;font-size:40px;text-align:center;}\r\n";        
  webSite+= "</style>\r\n";


  //javascript here:
  webSite+="<SCRIPT>\n";
  webSite+="xmlHttp=createXmlHttpObject();\n";

  webSite+="function createXmlHttpObject(){\n";
  webSite+="  if(window.XMLHttpRequest){\n";
  webSite+="    xmlHttp=new XMLHttpRequest();\n";
  webSite+="  }else{\n";
  webSite+="    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  webSite+="  }\n";
  webSite+="  return xmlHttp;\n";
  webSite+="}\n";

  webSite+="function process(){\n";
  webSite+="  if(xmlHttp.readyState==0||xmlHttp.readyState==4){\n";
  webSite+="    xmlHttp.onreadystatechange=function(){\n";
  webSite+="      if(xmlHttp.readyState==4&&xmlHttp.status==200){\n";
  webSite+="        xmlDoc=xmlHttp.responseXML;\n";
  
  webSite+="        xmlmsg=xmlDoc.getElementsByTagName('humidityval')[0].firstChild.nodeValue;\n";
  webSite+="        document.getElementById('humidity').innerHTML=xmlmsg;\n";
  webSite+="        xmlmsg=xmlDoc.getElementsByTagName('temperatureval')[0].firstChild.nodeValue;\n";
  webSite+="        document.getElementById('temperature').innerHTML=xmlmsg;\n";

  //Button1 update
  webSite+="          xmlmsg=xmlDoc.getElementsByTagName('button1val')[0].firstChild.nodeValue;\n";
  webSite+="          document.getElementById('buttonid1').value=xmlmsg;\n";
  webSite+="          xmlmsg=xmlDoc.getElementsByTagName('button1col')[0].firstChild.nodeValue;\n";
  webSite+="          document.getElementById('buttonid1').className=xmlmsg;\n";

  //Slider
  webSite+="        for(i=0;i<"+(String)sliderMAX+";i++){\n";
  webSite+="          xmlmsg=xmlDoc.getElementsByTagName('sliderval'+i)[0].firstChild.nodeValue;\n";
  webSite+="          document.getElementById('slider'+i).value=xmlmsg;\n";
  webSite+="          document.getElementById('Sliderval'+i).innerHTML=xmlmsg;\n";
  webSite+="          xmlmsg=xmlDoc.getElementsByTagName('ESPval'+i)[0].firstChild.nodeValue;\n";
  webSite+="          document.getElementById('ESPval'+i).innerHTML=xmlmsg;\n";
  webSite+="        }\n";
  webSite+="      }\n";
  webSite+="    }\n";
  webSite+="    xmlHttp.open('PUT','xml',true);\n";
  webSite+="    xmlHttp.send(null);\n";
  webSite+="  }\n";
  webSite+="  setTimeout('process()',5000);\n";
  webSite+="}\n";

  webSite+="function Slider(cnt){\n";
  webSite+="  sliderVal=document.getElementById('slider'+cnt).value;\n";
  webSite+="  document.getElementById('Sliderval'+cnt).innerHTML=sliderVal;\n";
  webSite+="  document.getElementById('ESPval'+cnt).innerHTML=9*(100-sliderVal)+100;\n";
  webSite+="  if(xmlHttp.readyState==0||xmlHttp.readyState==4){\n";
  webSite+="    xmlHttp.open('PUT','setESPval?cnt='+cnt+'&val='+sliderVal,true);\n";
  webSite+="    xmlHttp.send(null);\n";
  webSite+="  }\n";
  webSite+="}\n";


  webSite+="function Button1(){\n";
  webSite+="  if(xmlHttp.readyState==0||xmlHttp.readyState==4){\n";
  webSite+="    xmlHttp.open('POST','button1page',true);\n";
  webSite+="    xmlHttp.send(null);\n";
  webSite+="  }\n";
  webSite+="}\n";
 
  webSite+="</SCRIPT>\n";

  //end of javascript

  webSite+="<BODY onload='process()'>\n";
  webSite+="<h1>Dan's House</h1><BR>\n";

  webSite+="Humidity = <A ID='humidity'></A><BR>\n";
  webSite+="Temperature = <A ID='temperature'></A><BR><BR>\n";
  
  webSite+="<TABLE BORDER=1 style='text-align:center;border-collapse:collapse'>\n";
  for(int i=0;i<sliderMAX;i++){
    webSite+="<TR>\n";
    webSite+="<TD><BR><INPUT ID='slider"+(String)i+"' TYPE='range' ONCHANGE='Slider("+(String)i+")'></TD>\n";  //in Firefox, Chrome and Edge use ONINPUT 
    webSite+="<TD>Slidervalue"+(String)i+" = <A ID='Sliderval"+(String)i+"'></A><BR>\n";
    webSite+="ESPval"+(String)i+" = <A ID='ESPval"+(String)i+"'></A> milliseconds</TD>\n";
    webSite+="</TR>\n";
  }
  webSite+="</TABLE>\n";

  webSite+= "<div><p><input id='buttonid1' class='button-red' type='button' value='LED 1 On' onclick='Button1()'></div>";
    
  webSite+="</BODY>\n";
  webSite+="</HTML>\n";
}



void buildXML(){

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  XML="<?xml version='1.0'?>";
  XML+="<xml>";
  XML+="<humidityval>";
  XML+=String(h);
  XML+="</humidityval>";
  XML+="<temperatureval>";
  XML+=String(t);
  XML+="</temperatureval>";

if (digitalRead(LED2) == HIGH) {
  XML+="<button1val>";
  XML+="LED 1 OFF";
  XML+="</button1val>";
  XML+="<button1col>";
  XML+="button-blue";
  XML+="</button1col>";
}
else{
  XML+="<button1val>";
  XML+="LED 1 ON";
  XML+="</button1val>";
  XML+="<button1col>";
  XML+="button-red";
  XML+="</button1col>";
}
    
  for(int i=0;i<sliderMAX;i++){
    XML+="<sliderval"+(String)i+">";
    XML+=String(sliderVal[i]);
    XML+="</sliderval"+(String)i+">";
    XML+="<ESPval"+(String)i+">";
    ESPval[i]=9*(100-sliderVal[i])+100;
    XML+=String(ESPval[i]);
    XML+="</ESPval"+(String)i+">";
  }
  XML+="</xml>";
}

//Get uptime:
//String millis2time(){
//  String Time="";
//  unsigned long ss;
//  byte mm,hh;
//  ss=millis()/1000;
//  hh=ss/3600;
//  mm=(ss-hh*3600)/60;
//  ss=(ss-hh*3600)-mm*60;
//  if(hh<10)Time+="0";
//  Time+=(String)hh+":";
//  if(mm<10)Time+="0";
//  Time+=(String)mm+":";
//  if(ss<10)Time+="0";
//  Time+=(String)ss;
//  return Time;
//}

void handleWebsite(){
  buildWebsite();
  server.send(200,"text/html",webSite);
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}

void handleESPval(){
  int sliderCNT=server.arg("cnt").toInt();
  sliderVal[sliderCNT]=server.arg("val").toInt();
  buildXML();
  server.send(200,"text/xml",XML);
}

void handleButton1(){
  Serial.print("pressed");
  digitalWrite(LED2,!digitalRead(LED2));
  buildXML();
  server.send(200,"text/xml",XML);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  pinMode(LED2,OUTPUT);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED)delay(500);
  WiFi.mode(WIFI_STA);
  Serial.println("\n\nBOOTING ESP8266 ...");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Station IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/",handleWebsite);
  server.on("/xml",handleXML);
  server.on("/setESPval",handleESPval);
  server.on("/button1page",handleButton1);
  server.begin();
}

void loop() {
  server.handleClient();
  if(millis()>wait000){
    buildXML();
    wait000=millis()+1000UL;
  }
  if(millis()>wait001){
    digitalWrite(LED,!digitalRead(LED));
    wait001=millis()+ESPval[0];
  }
}
