var logEventFunc = function(id) { return document.getElementById(id); }; // To interact with elements on a WEB page
var btn_trigger_force_calc_hash = logEventFunc('btn.trigger.force.calc.hash'), msglog = logEventFunc('log'), msghash = logEventFunc('hash'); // ID of the elements in the HTML file
var isEnableElementsById = function(en) { btn_trigger_force_calc_hash.disabled  = !en; }; // The state of the elements  on the page

var logID = text => msglog.innerHTML += text + '<br/>\n';
var hashID = text => msghash.innerHTML += text + '<br/>\n';

isEnableElementsById(true);

btn_trigger_force_calc_hash.onclick = ev => fetch('/api/data').then(r => r.json()).then(r => {
    logID(JSON.stringify(r))
}).catch(err => console.log(err));