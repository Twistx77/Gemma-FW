#include "MW_Uploader.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>


WebServer server(80);


const char* serverIndex =
"<script src='/jquery.min.js'></script>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<p><font size=6><b>Gemma Firmware Update</b></font></p>"
    "<p><font >Select the new firmware file and press update.</b></font></p>"
    "<input type='file' style='margin-left:auto;margin-right:auto;display:block;margin-top:5%;margin-bottom:5%' name='update'>"
    "<input type='submit'style='margin-left:auto;margin-right:auto;display:block;margin-top:5%;margin-bottom:5%' value='Update'>"

    "</form>"
    "<center> <div  id='prg'>progress: 0%</div></center>"
    "<script>"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $('#upload_form')[0];"
    "var data = new FormData(form);"
    " $.ajax({"
    "url: '/update',"
    "type: 'POST',"
    "data: data,"
    "contentType: false,"
    "processData:false,"
    "xhr: function() {"
    "var xhr = new window.XMLHttpRequest();"
    "xhr.upload.addEventListener('progress', function(evt) {"
    "if (evt.lengthComputable) {"
    "var per = evt.loaded / evt.total;"
    "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
    "}"
    "}, false);"
    "return xhr;"
    "},"
    "success:function(d, s) {"
    "console.log('success!')"
    "},"
    "error: function (a, b, c) {"
    "}"
    "});"
    "});"
    "</script>";
    

/*
 * setup function
 */
void MWUP_EnterBootloaderMode()
{
  // Wifi Config
  const char *sw_version = "20191009";
  const char *host = "Gemma";
  const char *ssid = "Gemma";
  const char *password = "123456789";


  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://Gemma.local
    
    while (1) {
      
    }
  }
  /* 
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });*/
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();

  while(1)
  {
    server.handleClient();
    delay(1);
  }
  
}
