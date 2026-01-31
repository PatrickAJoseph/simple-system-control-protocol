def process_command(command):
    action = command.get("action")

    if action == "calculate":
        value = command.get("value", 0)
        return {"result": value * 2}

    return {"error": "Unknown command"}
