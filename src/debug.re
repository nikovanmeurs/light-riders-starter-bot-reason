let out_chan = stderr;
/* let out_chan = open_out "mybot_err.log";*/ 

/*
let debug s = 
  output_string out_chan s; 
  flush out_chan
;
*/

let debug s => ();
