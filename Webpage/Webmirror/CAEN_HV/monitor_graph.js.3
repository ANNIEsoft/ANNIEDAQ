// Load the Visualization API and the chart package.
google.charts.load('current', {'packages':['line', 'corechart']});

// Set a callback to run when the Google Visualization API is loaded. We are calling RequestData to do the initial data load.
google.charts.setOnLoadCallback(RequestData());

//This function should be the first to be called whenever data is refreshed. It sends the request to the web service.
function RequestData() {

	//Code snippet from w3c schools, handles different browsers correctly.
	var xmlhttp = null;
	if (window.XMLHttpRequest){
		// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	} else {
		// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
		
	//Sets up callback to handle response. This only executes once the server has a response.
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			//JSON.parse is more secure than eval as no scripts will run. May not be supported in all browsers though.
			var returndata = JSON.parse(xmlhttp.responseText);
			LoadData(returndata);
		} else {
			// wait for the call to complete
		}
	};
		
	//Sends the request.
	var channel_number = document.getElementById("channel_number_hidden").value;
	xmlhttp.open('GET','/CAEN_HV/Channel_Monitor/GetMonitorDatav2?Channel='+channel_number,true);
	xmlhttp.send(null);
}
	  
//This function takes the return data of our webservice and draws the chart on the page.
function LoadData(WebServiceData) {
	//Convert data to the correct format (in this case for a 2D array)
	//The .LVArray attribute is named this because the indicator name on the FP is this
	var GoogleTableData = LVArraytoGoogleDataTable(WebServiceData.LVArray);
	//var GoogleTableData = trialdata(WebServiceData.LVArray);
	  
	// Set chart options
	var options = { 
		chart: { title: 'Channel Monitoring History'}, 
		width: 900, 
		height: 500,
		series: { 
			0: {axis: 'Volts'}, 
			1: {axis: 'Volts'},
			2: {axis: 'Amps'}//, 
			//3: {axis: 'Amps'}
		},
		axes: {
			y: {
			  Volts: {label: 'Voltage (V)'}, 
			  Amps: {label: 'Current (uA)'}
			}
		}
	};

        // Instantiate and draw our chart, passing in some options.
        var chart = new google.charts.Line(document.getElementById('chart_div'));
        chart.draw(GoogleTableData, options);
}
	  
//This function takes a LV 2D array and converts it to a data table
function LVArraytoGoogleDataTable(lvarrayin){
	// Create the data table. X values will be timestamps. Y values will be Voltage/Current
       	var googletableout = new google.visualization.DataTable();
        googletableout.addColumn('datetime', 'Timestamp');
	googletableout.addColumn('number', 'Vmon');
        googletableout.addColumn('number', 'Vset');
	googletableout.addColumn('number', 'Imon');
	//googletableout.addColumn('number', 'Iset');
		
	//Add each row to the data table.
	//This parses through the arrays and puts each value in the columns of the data table.
	var numrows = lvarrayin.length;
	for(i=0; i<lvarrayin.length;i++)
	{
		googletableout.addRow([new Date(lvarrayin[i][0], lvarrayin[i][1], lvarrayin[i][2], lvarrayin[i][3], lvarrayin[i][4], lvarrayin[i][5]), lvarrayin[i][6], lvarrayin[i][7], ((lvarrayin[i][8])*1000000)]);//, ((lvarrayin[i][9])*1000000)]);
	}
	return googletableout;
};

function trialdata(lvarrayin){

     	var data = new google.visualization.DataTable();
     	data.addColumn('date', 'Month');
     	data.addColumn('number', "Average Temperature");
     	data.addColumn('number', "Average Hours of Daylight");
	data.addRows([
        [new Date(2014, 0),  -.5,  5.7],
        [new Date(2014, 1),   .4,  8.7],
        [new Date(2014, 2),   .5,   12],
        [new Date(2014, 3),  2.9, 15.3],
        [new Date(2014, 4),  6.3, 18.6],
        [new Date(2014, 5),    9, 20.9],
        [new Date(2014, 6), 10.6, 19.8],
        [new Date(2014, 7), 10.3, 16.6],
        [new Date(2014, 8),  7.4, 13.3],
        [new Date(2014, 9),  4.4,  9.9],
        [new Date(2014, 10), 1.1,  6.6],
        [new Date(2014, 11), -.2,  4.5]
    	]);
	return data;
};
	  