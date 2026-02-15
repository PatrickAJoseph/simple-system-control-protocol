let selectedFile = null;
const API_BASE = "http://localhost:3000";

document.getElementById("fileInput").addEventListener("change", (e) => {
  selectedFile = e.target.files[0];
});

document.getElementById("loadBtn").addEventListener("click", loadYaml);

function loadYaml() {
  if (!selectedFile) {
    showAlert("Please select a YAML file first");
    return;
  }
  const reader = new FileReader();
  reader.onload = (e) => {
    console.log("inside reader.onload");
    try {
      const parsed = jsyaml.load(e.target.result);
      console.log("Parsed YAML:", parsed);
      populateTable(parsed);
    } catch (err) {
      console.error(err);
      showAlert("Invalid YAML format");
    }
  }
  reader.readAsText(selectedFile);
}
function populateTable(parsedData) {
  const tbody = document.querySelector("#registerTable tbody");
  tbody.innerHTML = "";

  if (!parsedData.regs) {
    showAlert("No 'regs' section found in YAML")
    return;
  }

  const registers = parsedData.regs;

  Object.entries(registers).forEach(([regKey, regObj]) => {

    const registerName = regObj.name;

    Object.entries(regObj).forEach(([key, value]) => {

      // Only process parameters
      if (key.startsWith("parameter_")) {

        const paramName = value.name;

        const tr = document.createElement("tr");

        tr.innerHTML = `
          <td>${registerName}</td>
          <td>${paramName}</td>
          <td>
            <input type="number" value="0">
          </td>
          <td>
            <button onclick="getValue('${regKey}', '${paramName}')">
              GET
            </button>
          </td>
          <td>
            <button onclick="setValue('${regKey}', '${paramName}', this)">
              SET
            </button>
          </td>
        `;

        tbody.appendChild(tr);
      }

    });

  });
}

function showAlert(title, message) {
  document.getElementById("modalTitle").innerText = title;
  if(message){
    document.getElementById("modalMessage").innerText = message;
  }
  document.getElementById("customModal").style.display = "block";
}

function closeModal() {
  document.getElementById("customModal").style.display = "none";
}


/* Placeholder handlers */
// function getValue(reg, param) {
//   showAlert("Register Read", `GET ${reg} → ${param}`);
// }

// function setValue(reg, param, btn) {
//   const value = btn.closest("tr").querySelector("input").value;
//   showAlert("Register Set", `SET ${reg} → ${param}`);
// }

async function getValue(reg, param) {
  console.log ("Get request is called");
  try {
    const response = await fetch(`${API_BASE}/api/get/${reg}/${param}`, {
      method: "GET",
    });

    const data = await response.json();
    console.log("res: ", data);
    alert(`Value: ${data.result.result}`);
  } catch (err) {
    console.error(err);
    alert("GET request failed");
  }
}

async function setValue(reg, param, btn) {
  const value = btn.closest("tr").querySelector("input").value;

  try {
    const response = await fetch(`${API_BASE}/api/set`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        type: "set",
        register: reg,
        parameter: param,
        value: value
      })
    });

    const data = await response.json();
    console.log("data: ", data);
    alert(data.result);
  } catch (err) {
    console.error(err);
    alert("SET request failed");
  }
}
