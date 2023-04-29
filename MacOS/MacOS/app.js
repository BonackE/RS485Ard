window.onerror = (msg, url, line, column, error) => {
  const message = {
    message: msg,
    url: url,
    line: line,
    column: column,
    error: JSON.stringify(error),
  }

  if (window.webkit) {
    window.webkit.messageHandlers.error.postMessage(message)
  } else {
    console.log('Error:', message)
  }
}

function display(data) {
  if (window.webkit) {
    window.webkit.messageHandlers.console.postMessage(data)
  } else {
    console.log('Data:', data)
  }
}

// helper function to parse an int to CCHAR data type
function parseInt8(value) {
  const intValue = parseInt(value)
  return intValue > 127 ? intValue - 256 : intValue
}

// formatting data to send to controller
const form = document.getElementById('modbus-form')
form.addEventListener('submit', function (event) {
  event.preventDefault()

  const formData = new FormData(form)

  const numericFields = [
    'baudRate',
    'dataBits',
    'stopBits',
    'slaveId',
    'functionCode',
    'startAddr',
    'numCoils',
  ]
  const int8Field = ['parity']

  const data = {}

  for (let field of formData.entries()) {
    if (numericFields.includes(field[0])) {
      const value = parseInt(field[1])
      if (isNaN(value)) {
        // Handle invalid number value here, such as throwing an error or setting a default value
        display('invalid number value')
      } else {
        data[field[0]] = value
      }
    } else {
      if (int8Field.includes(field[0])) {
        const value = field[1].charCodeAt(0)
        data[field[0]] = value
      } else {
        data[field[0]] = field[1]
      }
    }
  }
  const jsonData = JSON.stringify(data)
  display(jsonData)

  window.webkit.messageHandlers.Connect.postMessage(jsonData)
})

// call to Swift to LoadPorts()
function LoadPorts() {
  var docPorts = document.getElementsByName('serialPort')
  if (docPorts[0].options.length > 0) removeOptions(docPorts[0])

  if (window.webkit) {
    window.webkit.messageHandlers.LoadPorts.postMessage('')
  }
}

// refresh options list
function removeOptions(selectElement) {
  var i,
    L = selectElement.options.length - 1
  for (i = L; i >= 0; i--) {
    selectElement.remove(i)
  }
}

// called by WK with data to populate the webview
function handlePortsData(ports) {
  var docPorts = document.getElementsByName('serialPort')
  for (var i = 0; i < ports.length; i++) {
    var optn = document.createElement('OPTION')
    optn.text = '/dev/' + ports[i]
    optn.value = '/dev/' + ports[i]
    docPorts[0].options.add(optn)
  }
}

// display a log for the user
function displayUserLog(data) {
        if (typeof data === 'string') {
           document.getElementById('userLog').innerHTML = data;
       } else {
           console.error('Error: displayUserLog() expected a string but received ' + typeof data);
       }
}
