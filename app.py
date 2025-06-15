import os
import json
import sqlite3
import pandas as pd
from datetime import datetime
from flask import Flask, request, jsonify, render_template, send_file
from pytz import timezone

app = Flask(__name__, static_folder='static')

# Database setup
DB_PATH = 'sensor_data.db'

def init_db():
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS sensor_data (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        timestamp TEXT NOT NULL,
        ldr REAL,
        pir INTEGER,
        current REAL,
        battery_voltage REAL,
        battery_level REAL,
        solar_voltage REAL
    )
    ''')
    conn.commit()
    conn.close()

# Initialize the database
init_db()

@app.route('/')
def index():
    """Serve the main dashboard page"""
    return render_template('index.html')

@app.route('/data', methods=['POST'])
def receive_data():
    """Endpoint to receive and save data from ESP32"""
    try:
        data = request.json
        
        # Validate required fields
        required_fields = ['ldr', 'pir', 'current', 'battery_voltage', 'battery_level', 'solar_voltage']
        for field in required_fields:
            if field not in data:
                return jsonify({"status": "error", "message": f"Missing required field: {field}"}), 400
        
        # Connect to database
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()
        
        # Insert data with current timestamp in ISO format and the correct timezone
        current_time = datetime.now(timezone('Asia/Kolkata')).strftime('%Y-%m-%d %H:%M:%S')
        print(f"DEBUG: Saving data with timestamp: {current_time}")
        
        cursor.execute('''
        INSERT INTO sensor_data (timestamp, ldr, pir, current, battery_voltage, battery_level, solar_voltage)
        VALUES (?, ?, ?, ?, ?, ?, ?)
        ''', (
            current_time,
            data['ldr'],
            data['pir'],
            data['current'],
            data['battery_voltage'],
            data['battery_level'],
            data['solar_voltage']
        ))
        
        conn.commit()
        conn.close()
        
        return jsonify({"status": "success", "message": "Data saved successfully"}), 201
    
    except Exception as e:
        print(f"DEBUG: Error occurred - {str(e)}")
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/get_data', methods=['GET'])
def get_data():
    """Endpoint to get the latest sensor data"""
    try:
        conn = sqlite3.connect(DB_PATH)
        
        # Get all data for charts
        df = pd.read_sql_query("SELECT * FROM sensor_data ORDER BY timestamp DESC", conn)
        
        # Convert to dictionary format for JSON response
        records = df.to_dict('records')
        
        # Get just the latest record
        latest = None
        if not df.empty:
            latest = df.iloc[0].to_dict()
        
        conn.close()
        
        return jsonify({
            "status": "success",
            "latest": latest,
            "records": records
        })
    
    except Exception as e:
        print(f"DEBUG: Error occurred - {str(e)}")
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/download', methods=['GET'])
def download_data():
    """Endpoint to download data as Excel file"""
    try:
        conn = sqlite3.connect(DB_PATH)
        df = pd.read_sql_query("SELECT * FROM sensor_data", conn)
        conn.close()
        
        # Create Excel file
        output_file = 'sensor_data_export.xlsx'
        df.to_excel(output_file, index=False)
        
        # Return file for download
        return send_file(output_file, as_attachment=True)
    
    except Exception as e:
        print(f"DEBUG: Error occurred - {str(e)}")
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/reset', methods=['POST'])
def reset_data():
    """Endpoint to clear the database"""
    try:
        conn = sqlite3.connect(DB_PATH)
        cursor = conn.cursor()
        
        # Delete all data from the table
        cursor.execute("DELETE FROM sensor_data")
        
        conn.commit()
        conn.close()
        
        return jsonify({"status": "success", "message": "Database reset successfully"})
    
    except Exception as e:
        print(f"DEBUG: Error occurred - {str(e)}")
        return jsonify({"status": "error", "message": str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
