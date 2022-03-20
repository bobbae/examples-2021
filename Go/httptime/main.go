/* tick-tock.go */
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
              <script src="//code.jquery.com/jquery-1.11.1.min.js"></script>
          </head>
           <body>
            <div id="output" ></div>
             <script type="text/javascript">
               $(document).ready(function () {
                 $("#output").append("Waiting for system time..");
                 setInterval("delayedPost()", 1000);
               });
               function delayedPost() {
                 $.post("http://localhost:8080/gettime", "", function(data, status) {
                 $("#output").empty();
                 $("#output").append(data);
                 });
               }
             </script>
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
	log.Fatal(http.ListenAndServe(":8080", nil))
}
