import json
import os
from flask import Flask, jsonify, abort, render_template, request, send_from_directory
app = Flask(__name__)

@app.route('/')
def hello_world():
    # Servir la page HTML du dashboard
    return send_from_directory(os.path.dirname(os.path.abspath(__file__)), 'index.html')

welcome = "Welcome to 3ESE API!"

@app.route('/api/welcome/')
def api_welcome():
    return welcome

@app.route('/api/welcome/<int:index>', methods=['GET', 'POST', 'PUT', 'DELETE'])
def api_welcome_index(index):
    global welcome
    
    if request.method == 'GET':
        if index >= len(welcome):
            abort(404)
        else:
            return jsonify({"index": index, "val": welcome[index]})
    
    elif request.method == 'POST':
        data = request.get_json()
        if data is None or 'val' not in data:
            abort(400)  # Bad Request
        # Insert at index
        welcome = welcome[:index] + data['val'] + welcome[index:]
        return '', 204  # No Content
    
    elif request.method == 'PUT':
        data = request.get_json()
        if data is None or 'val' not in data:
            abort(400)  # Bad Request
        if index >= len(welcome):
            abort(404)
        # Replace character at index
        welcome = welcome[:index] + data['val'] + welcome[index+1:]
        return '', 204  # No Content
    
    elif request.method == 'DELETE':
        if index >= len(welcome):
            abort(404)
        # Remove character at index
        welcome = welcome[:index] + welcome[index+1:]
        return '', 204  # No Content

@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET','POST'])
def api_request(path=None):
    resp = {
            "method":   request.method,
            "url" :  request.url,
            "path" : path,
            "args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
        resp["POST"] = {
                "data" : request.get_json(),
                }
    return jsonify(resp)

# API REST pour températures, pressions, échelles et angles
temperatures = []
pressures = []
scale = 1.0  # Échelle par défaut en Kelvin

# POST /temp/ - Récupérer une nouvelle température
@app.route('/temp/', methods=['POST'])
def create_temperature():
    global temperatures
    # Pour l'instant, on simule une récupération de température
    # Dans un vrai cas, on lirait depuis le capteur BMP280
    data = request.get_json()
    if data and 'temp' in data:
        temp = data['temp']
    else:
        # Valeur par défaut si non fournie (simulation)
        temp = 20.0  # Température en °C
    
    temperatures.append(temp)
    return '', 204  # No Content

# POST /pres/ - Récupérer une nouvelle pression
@app.route('/pres/', methods=['POST'])
def create_pressure():
    global pressures
    # Pour l'instant, on simule une récupération de pression
    # Dans un vrai cas, on lirait depuis le capteur BMP280
    data = request.get_json()
    if data and 'pres' in data:
        pres = data['pres']
    else:
        # Valeur par défaut si non fournie (simulation)
        pres = 1013.25  # Pression en hPa
    
    pressures.append(pres)
    return '', 204  # No Content

# GET /temp/ - Retourner toutes les températures précédentes
@app.route('/temp/', methods=['GET'])
def get_all_temperatures():
    return jsonify(temperatures)

# GET /temp/x - Retourner la température #x
@app.route('/temp/<int:x>', methods=['GET'])
def get_temperature(x):
    if x < 0 or x >= len(temperatures):
        abort(404)
    return jsonify({"index": x, "temp": temperatures[x]})

# GET /pres/ - Retourner toutes les pressions précédentes
@app.route('/pres/', methods=['GET'])
def get_all_pressures():
    return jsonify(pressures)

# GET /pres/x - Retourner la pression #x
@app.route('/pres/<int:x>', methods=['GET'])
def get_pressure(x):
    if x < 0 or x >= len(pressures):
        abort(404)
    return jsonify({"index": x, "pres": pressures[x]})

# GET /scale/ - Retourner l'échelle (K)
@app.route('/scale/', methods=['GET'])
def get_scale():
    return jsonify({"scale": scale})

# GET /angle/ - Retourner l'angle (temp x scale)
@app.route('/angle/', methods=['GET'])
def get_angle():
    if len(temperatures) == 0:
        abort(404)  # Pas de température disponible
    # Utiliser la dernière température
    last_temp = temperatures[-1]
    angle = last_temp * scale
    return jsonify({"temp": last_temp, "scale": scale, "angle": angle})

# POST /scale/x - Changer l'échelle (K) pour x
@app.route('/scale/<int:x>', methods=['POST'])
def update_scale(x):
    global scale
    data = request.get_json()
    if data is None or 'scale' not in data:
        abort(400)  # Bad Request
    
    new_scale = data['scale']
    if not isinstance(new_scale, (int, float)):
        abort(400)  # Bad Request
    
    scale = float(new_scale)
    return '', 204  # No Content

# DELETE /temp/x - Supprimer la température #x
@app.route('/temp/<int:x>', methods=['DELETE'])
def delete_temperature(x):
    global temperatures
    if x < 0 or x >= len(temperatures):
        abort(404)
    temperatures.pop(x)
    return '', 204  # No Content

# DELETE /pres/x - Supprimer la pression #x
@app.route('/pres/<int:x>', methods=['DELETE'])
def delete_pressure(x):
    global pressures
    if x < 0 or x >= len(pressures):
        abort(404)
    pressures.pop(x)
    return '', 204  # No Content
