var logEventFunc = function(id) { return document.getElementById(id); }; // To interact with elements on a WEB page
var btn_trigger_force_calc_hash = logEventFunc('btn.trigger.force.calc.hash'), msglog = logEventFunc('log'); // ID of the elements in the HTML file
var isEnableElementsById = function(en) { btn_trigger_force_calc_hash.disabled  = !en; }; // The state of the elements  on the page

var lodg = text => msglog.innerHTML += text + '<br/>\n';

isEnableElementsById(true);

btn_trigger_force_calc_hash.onclick = ev => fetch('/api/hello').then(r => r.json()).then(r => {
    console.log(r);
    lodg(JSON.stringify(r))
}).catch(err => console.log(err));