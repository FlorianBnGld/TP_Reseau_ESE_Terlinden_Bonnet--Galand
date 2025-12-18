import json
import os
import time
import serial
from flask import Flask, jsonify, abort, render_template, request, send_from_directory

app = Flask(__name__)

# --- CONFIGURATION UART RASPBERRY PI ---
# '/dev/ttyS0' est généralement le port sur les GPIO (Pin 8 TX, Pin 10 RX)
# Assure-toi que les masses (GND) sont reliées !
SERIAL_PORT = '/dev/ttyAMA0' 
BAUD_RATE = 115200 

ser = None
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Port série {SERIAL_PORT} ouvert.")
except Exception as e:
    print(f"Erreur série : {e}")

@app.route('/')
def hello_world():
    return render_template('index.html')

welcome = "Welcome to 3ESE API!"

# ... (Tes routes /api/welcome/ restent inchangées) ...

@app.route('/api/welcome/')
def api_welcome():
    return welcome

@app.route('/api/welcome/<int:index>', methods=['GET', 'POST', 'PUT', 'DELETE'])
def api_welcome_index(index):
    global welcome
    if request.method == 'GET':
        if index >= len(welcome): abort(404)
        else: return jsonify({"index": index, "val": welcome[index]})
    elif request.method == 'POST':
        data = request.get_json()
        if data is None or 'val' not in data: abort(400)
        welcome = welcome[:index] + data['val'] + welcome[index:]
        return '', 204
    elif request.method == 'PUT':
        data = request.get_json()
        if data is None or 'val' not in data: abort(400)
        if index >= len(welcome): abort(404)
        welcome = welcome[:index] + data['val'] + welcome[index+1:]
        return '', 204
    elif request.method == 'DELETE':
        if index >= len(welcome): abort(404)
        welcome = welcome[:index] + welcome[index+1:]
        return '', 204

@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404

# ... (Tes tableaux globaux) ...
temperatures = []
pressures = []
scale = 1.0

# --- MODIFICATION MAJEURE ICI ---
@app.route('/temp/', methods=['POST'])
def create_temperature():
    global temperatures
    temp_val = None

    if ser and ser.is_open:
        try:
            # 1. Vider le tampon pour éviter de lire de vieilles données
            ser.reset_input_buffer()
            
            # 2. Envoyer la commande 't' suivie de Entrée (\r) pour déclencher GET_T
            # Le shell STM32 attend que l'utilisateur appuie sur 't'
            ser.write(b't\r') 
            
            # 3. Lire la réponse
            # Ta fonction GET_T fait : printf("%.2f\n", new_t);
            # C'est la première chose qu'elle envoie.
            
            # On lit quelques lignes pour trouver le nombre (au cas où le shell fait de l'écho)
            for _ in range(3): 
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                # On cherche une ligne qui ressemble à un nombre (ex: "24.50")
                if line:
                    try:
                        temp_val = float(line)
                        print(f"STM32 a répondu : {temp_val}")
                        break # On a trouvé, on sort de la boucle
                    except ValueError:
                        # Ce n'était pas un nombre (peut-être l'écho de la commande 't' ou un log debug)
                        continue 
                        
        except Exception as e:
            print(f"Erreur communication STM32: {e}")

    # Si UART échoue, valeur par défaut pour ne pas planter l'API
    if temp_val is None:
        # On regarde si on a reçu du JSON (simulation)
        data = request.get_json(silent=True)
        if data and 'temp' in data:
            temp_val = data['temp']
        else:
            temp_val = -99.9 # Code d'erreur visible

    temperatures.append(temp_val)
    
    # On retourne la valeur lue pour confirmer (code 201 Created)
    return jsonify({"temp": temp_val}), 201

# ... (Le reste de tes routes reste identique) ...

@app.route('/pres/', methods=['POST'])
def create_pressure():
    global pressures
    data = request.get_json(silent=True)
    if data and 'pres' in data:
        pres = data['pres']
    else:
        pres = 1013.25
    pressures.append(pres)
    return '', 204

@app.route('/temp/', methods=['GET'])
def get_all_temperatures():
    return jsonify(temperatures)

@app.route('/temp/<int:x>', methods=['GET'])
def get_temperature(x):
    if x < 0 or x >= len(temperatures): abort(404)
    return jsonify({"index": x, "temp": temperatures[x]})

@app.route('/pres/', methods=['GET'])
def get_all_pressures():
    return jsonify(pressures)

@app.route('/pres/<int:x>', methods=['GET'])
def get_pressure(x):
    if x < 0 or x >= len(pressures): abort(404)
    return jsonify({"index": x, "pres": pressures[x]})

@app.route('/scale/', methods=['GET'])
def get_scale():
    return jsonify({"scale": scale})

@app.route('/scale/<int:x>', methods=['POST'])
def update_scale(x):
    global scale
    data = request.get_json()
    if data is None or 'scale' not in data: abort(400)
    new_scale = data['scale']
    if not isinstance(new_scale, (int, float)): abort(400)
    scale = float(new_scale)
    return '', 204

@app.route('/temp/<int:x>', methods=['DELETE'])
def delete_temperature(x):
    global temperatures
    if x < 0 or x >= len(temperatures): abort(404)
    temperatures.pop(x)
    return '', 204

@app.route('/pres/<int:x>', methods=['DELETE'])
def delete_pressure(x):
    global pressures
    if x < 0 or x >= len(pressures): abort(404)
    pressures.pop(x)
    return '', 204

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000, debug=True)