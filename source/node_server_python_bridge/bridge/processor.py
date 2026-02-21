def process_command(type, reg, param, value = 10):
    if type == "get":
        return value
    elif type == "set":
        return value
    return {"error": "Unknown command"}
