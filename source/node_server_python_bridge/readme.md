# Project Execution Guide

This project consists of three components:

1. Python Bridge (Socket Server)
2. Node.js Backend (API Server)
3. Frontend (Web Interface)

All three components must be started in the correct order.

---

## How to Run the Project

### Step 1: Start Python Bridge

```code
cd source/node_server_python_bridge/bridge
python executor.py
```
### Step 2: Start Node Backend

```code
cd source/node_server_python_bridge/backend
npm install (Optional)
npm start
```

### Step 3: Running the Frontend
```
cd frontend
npx serve
```
OR, 

1. Install the **Live Server** extension in Visual Studio Code.
2. Open the `frontend` folder in VS Code.
3. Right-click on `index.html`.
4. Select **Open with Live Server**.
