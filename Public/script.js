
function sendTreat() {

  document.getElementById("boxTxt").innerHTML = `<div class="spinner-border" role="status"></div>`;
  document.getElementById("okBtn").disabled = true;
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        // Typical action to be performed when the document is ready:
          var response = xhttp.responseText;
          document.getElementById("boxTxt").innerHTML = response;
          document.getElementById("okBtn").disabled = false;
      }
  };
  xhttp.open("POST", "https://doggie.katienaha.com/treats", true);
  var params = 'timesDispense=1&timesPlayMusic=1&secondsDispense=0&secondsWaitMin=0&secondsWaitMax=25';
  xhttp.send(params);
  const btn = document.getElementById("activateBtn")
  btn.disabled = true;
  btn.textContent = 'Wait to Dispense a Treat...';
  setTimeout(()=>{
    btn.disabled = false;
    btn.textContent = 'Dispense a Treat';
    console.log('Button Activated')}, 10000)
  
}

function sendTwoMinsTreatsSilent() {

  document.getElementById("boxTxt").innerHTML = `<div class="spinner-border" role="status"></div>`;
  document.getElementById("okBtn").disabled = true;
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        // Typical action to be performed when the document is ready:
          var response = xhttp.responseText;
          document.getElementById("boxTxt").innerHTML = response;
          document.getElementById("okBtn").disabled = false;
      }
  };
  xhttp.open("POST", "https://doggie.katienaha.com/treats", true);
  var params = 'timesDispense=2&timesPlayMusic=0&secondsDispense=120&secondsWaitMin=15&secondsWaitMax=25';
  xhttp.send(params);
  
}


function sendFiveMinsTreatsSilent() {

  document.getElementById("boxTxt").innerHTML = `<div class="spinner-border" role="status"></div>`;
  document.getElementById("okBtn").disabled = true;
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        // Typical action to be performed when the document is ready:
          var response = xhttp.responseText;
          document.getElementById("boxTxt").innerHTML = response;
          document.getElementById("okBtn").disabled = false;
      }
  };
  xhttp.open("POST", "https://doggie.katienaha.com/treats", true);
  var params = 'timesDispense=2&timesPlayMusic=0&secondsDispense=300&secondsWaitMin=15&secondsWaitMax=25';
  xhttp.send(params);
  
}


