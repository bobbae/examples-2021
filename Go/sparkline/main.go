package main

import (
	"fmt"
	"log"
	"net/http"
	"time"
)

// Content for the main html page..
var page = `<html>
<head>
<script src="//ajax.googleapis.com/ajax/libs/jquery/1.11.1/jquery.min.js"></script>
<script src="http://omnipotent.net/jquery.sparkline/2.1.2/jquery.sparkline.js"></script>
<script type="text/javascript">
    $(function() {
        /** This code runs when everything has been loaded on the page */
        /* Inline sparklines take their values from the contents of the tag */
        $('.inlinesparkline').sparkline(); 

        /* Sparklines can also take their values from the first argument 
        passed to the sparkline() function */
        var myvalues = [10,8,5,7,4,4,1,39,40,39,4,45,5,5,34,5,35,45,34,54,24];
        $('.dynamicsparkline').sparkline(myvalues);

        /* The second argument gives options such as chart type */
        $('.dynamicbar').sparkline(myvalues, {type: 'bar', barColor: 'green'} );

        /* Use 'html' instead of an array of values to pass options 
        to a sparkline with data in the tag */
        $('.inlinebar').sparkline('html', {type: 'bar', barColor: 'red'} );

        var mrefreshinterval = 500; // update display every 500ms
        var lastmousex=-1; 
        var lastmousey=-1;
        var lastmousetime;
        var mousetravel = 0;
        var mpoints = [];
        var mpoints_max = 30;
        $('html').mousemove(function(e) {
            var mousex = e.pageX;
            var mousey = e.pageY;
            if (lastmousex > -1) {
                mousetravel += Math.max( Math.abs(mousex-lastmousex), Math.abs(mousey-lastmousey) );
            }
            lastmousex = mousex;
            lastmousey = mousey;
        });
        var mdraw = function() {
            var md = new Date();
            var timenow = md.getTime();
            if (lastmousetime && lastmousetime!=timenow) {
                var pps = Math.round(mousetravel / (timenow - lastmousetime) * 1000);
                mpoints.push(pps);
                if (mpoints.length > mpoints_max)
                    mpoints.splice(0,1);
                mousetravel = 0;
                $('#mousespeed').sparkline(mpoints, { width: mpoints.length*2, tooltipSuffix: ' pixels per second' });
            }
            lastmousetime = timenow;
            setTimeout(mdraw, mrefreshinterval);
        }
        // We could use setInterval instead, but I prefer to do it this way
        setTimeout(mdraw, mrefreshinterval); 
    });
</script>
</head>
<body>
<p>
Inline Sparkline: <span class="inlinesparkline">1,4,4,7,5,9,10</span>.
</p>
<p>
Sparkline with dynamic data: <span class="dynamicsparkline">Loading..</span> with more data <span class="dynamicbar"></span> and mouspeed <span id="mousespeed"></span> and more text.
</p>
<p>
Bar chart with dynamic data: <span class="dynamicbar">Loading..</span>
</p>
<p>
Bar chart with inline data: <span class="inlinebar">1,3,4,5,3,5</span>
</p>
</body>
</html>`

// handler for the main page.
func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprint(w, page)
}

// handler to cater AJAX requests
func handlerGetTime(w http.ResponseWriter, r *http.Request) {
	fmt.Fprint(w, time.Now().Format("Mon, 02 Jan 2006 15:04:05 MST"))
}

func main() {
	http.HandleFunc("/", handler)
	http.HandleFunc("/gettime", handlerGetTime)
	log.Fatal(http.ListenAndServe(":9999", nil))
}
