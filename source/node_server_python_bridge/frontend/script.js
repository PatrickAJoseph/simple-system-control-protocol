async function sendCommand() {
  const action = document.getElementById("commandInput").value;
  const value = document.getElementById("valueInput").value;

  const output = document.getElementById("output");
  output.textContent = "Sending...";

  try {
    const response = await fetch("http://localhost:3000/api/command", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        action: action,
        value: Number(value)
      })
    });

    const data = await response.json();
    output.textContent = JSON.stringify(data, null, 2);

  } catch (error) {
    output.textContent = "Error connecting to server";
  }
}
