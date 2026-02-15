def process_command(type, reg, param, value = 0):
    if type == "get":
        return {reg+param}
    elif type == "set":
        return f"set value {value}"
    return {"error": "Unknown command"}
