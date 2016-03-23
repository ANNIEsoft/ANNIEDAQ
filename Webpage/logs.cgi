#!/bin/bash
echo "

<head>
<meta http-equiv=\"refresh\" content=\"10\"><meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" /> <title>Logs</title>

</head>

<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/icon?family=Material+Icons\">
<script defer src=\"https://code.getmdl.io/1.1.2/material.min.js\"></script>
 <link rel=\"stylesheet\" href=\"https://code.getmdl.io/1.1.2/material.indigo-deep_purple.min.css\" />
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">

  <link rel=\"stylesheet\" href=\"styles.css\"> 
<body>

<!-- Always shows a header, even in smaller screens. -->
<div class=\"mdl-layout mdl-js-layout mdl-layout--fixed-header\">
  <header class=\"mdl-layout__header\">
    <div class=\"mdl-layout__header-row\">
      <!-- Title -->
      <span class=\"mdl-layout-title\">
Logs</span>
      <!-- Add spacer, to align navigation to the right -->
      <div class=\"mdl-layout-spacer\"></div>
      <!-- Navigation. We hide it in small screens. -->
      <nav class=\"mdl-navigation mdl-layout--large-screen-only\">
 <a class=\"mdl-navigation__link\" href=\"../index.html\">Home</a>
       <a class=\"mdl-navigation__link\" href=\"/cgi-bin/control.cgi\">Control</a>
        <a class=\"mdl-navigation__link\" href=\"/cgi-bin/logs.cgi\">Logs</a>
        <a class=\"mdl-navigation__link\" href=\"/cgi-bin/monitoring.cgi\">Monitoring</a>
        <a class=\"mdl-navigation__link\" href=\"/cgi-bin/SQL.cgi\">SQL</a>
        <a class=\"mdl-navigation__link\" href=\"/cgi-bin/data.cgi\">Data</a>
      </nav>
    </div>
  </header>
  <div class=\"mdl-layout__drawer\">
 <img src=\"../ANNIE-logo.png\"  width=\"90%\" height=\"10%\"> 
    <span class=\"mdl-layout-title\"></span>
<div class=\"android-drawer-separator\"></div>
    <nav class=\"mdl-navigation\">
<a class=\"mdl-navigation__link\" href=\"../index.html\">Home</a>
      <a class=\"mdl-navigation__link\" href=\"/cgi-bin/control.cgi\">Control</a>
        <a class=\"mdl-navigation__link\" href=\"/cgi-bin/logs.cgi\">Logs</a>
	<a class=\"mdl-navigation__link\" href=\"/cgi-bin/monitoring.cgi\">Monitoring</a>
        <a class=\"mdl-navigation__link\" href=\"/cgi-bin/SQL.cgi\">SQL</a>
	<a class=\"mdl-navigation__link\" href=\"/cgi-bin/data.cgi\">Data</a>
    </nav>
  </div>
  <main class=\"mdl-layout__content\">
    <div class=\"page-content\">
<!-- Your content goes here -->

"
for file in `ls /data/logs`
do
    echo "<a href=\"/logs/$file\"> $file </a>                                                                                                              

 
<div id=$file align='left' style=\"#ccc;font:12px/13px Georgia, Garamond, Serif;overflow:scroll;border:2px solid;padding:1%;height:200px\">"

    tail /data/logs/$file > /tmp/$file -n 15

    while IFS='' read -r line || [[ -n "$line" ]]; do
        echo "$line <br />"
    done <  /tmp/$file

  echo "</div><p>                                                                                                                                                                                                                                                                                              
<script type=\"text/javascript\">                                                                                                                           
var divid = document.getElementById(\"$file\");                                                                                                             
divid.scrollTop = divid.scrollHeight;"
#divid.scrollLeft = divid.scrollWidth                                                                                                                       
    echo "</script>"
done


echo "



  <footer class=\"mdl-mega-footer\">                                                                                                     
                                                                                                                                                            
          <div class=\"mdl-mega-footer--top-section\">                                                                                                        
            <div class=\"mdl-mega-footer--right-section\">                                                                                                    
              <a class=\"mdl-typography--font-light\" href=\"#top\">                                                                                            
                Back to Top                                                                                                                                 
                <i class=\"material-icons\">expand_less</i>                                                                                                   
              </a>                                                                                                                                          
            </div>                                                                                                                                          
          </div>                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            
          <div class=\"mdl-mega-footer--middle-section\">                                                                                                     
            <p class=\"mdl-typography--font-light\">ANNIE Collaboration © 2016 </p>                                                                           
            <p class=\"mdl-typography--font-light\">Created by Dr. B.Richards (b.richards@qmul.ac.uk)</p>                                                     
          </div>                                                                                                                                            
                                                                                                                                                            
                                                                                                                                                            
          </footer>   



</div>
  </main>
</div>

</body>
"                                                                                                                              